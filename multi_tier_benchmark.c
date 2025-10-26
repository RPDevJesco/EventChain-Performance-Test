#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 600

#include "eventchains.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* ==================== Timing Infrastructure ==================== */

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
static inline uint64_t get_time_ns(void) {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (uint64_t)((counter.QuadPart * 1000000000ULL) / frequency.QuadPart);
}
#elif defined(__MACH__) && defined(__APPLE__)
#include <mach/mach_time.h>
static inline uint64_t get_time_ns(void) {
    static mach_timebase_info_data_t timebase;
    if (timebase.denom == 0) {
        mach_timebase_info(&timebase);
    }
    return (mach_absolute_time() * timebase.numer) / timebase.denom;
}
#elif defined(CLOCK_MONOTONIC)
static inline uint64_t get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}
#else
#warning "Using low-precision clock() for timing"
static inline uint64_t get_time_ns(void) {
    return (uint64_t)(clock() * 1000000000ULL / CLOCKS_PER_SEC);
}
#endif

/* ==================== Benchmark Statistics ==================== */

typedef struct {
    uint64_t min_ns;
    uint64_t max_ns;
    uint64_t avg_ns;
    uint64_t total_ns;
    size_t iterations;
    double std_dev;
} BenchStats;

static void stats_init(BenchStats *stats) {
    stats->min_ns = UINT64_MAX;
    stats->max_ns = 0;
    stats->avg_ns = 0;
    stats->total_ns = 0;
    stats->iterations = 0;
    stats->std_dev = 0.0;
}

static void stats_add_sample(BenchStats *stats, uint64_t sample_ns) {
    if (sample_ns < stats->min_ns) stats->min_ns = sample_ns;
    if (sample_ns > stats->max_ns) stats->max_ns = sample_ns;
    stats->total_ns += sample_ns;
    stats->iterations++;
}

static void stats_finalize(BenchStats *stats, uint64_t *samples) {
    if (stats->iterations > 0) {
        stats->avg_ns = stats->total_ns / stats->iterations;
        
        /* Calculate standard deviation */
        double variance = 0.0;
        for (size_t i = 0; i < stats->iterations; i++) {
            double diff = (double)samples[i] - (double)stats->avg_ns;
            variance += diff * diff;
        }
        variance /= stats->iterations;
        stats->std_dev = sqrt(variance);
    }
}

static void stats_print(const char *name, BenchStats *stats) {
    printf("%-35s: avg=%7.3f us  min=%7.3f us  max=%7.3f us  stddev=%6.3f us\n",
           name,
           stats->avg_ns / 1000.0,
           stats->min_ns / 1000.0,
           stats->max_ns / 1000.0,
           stats->std_dev / 1000.0);
}

static void stats_print_comparison(const char *name, BenchStats *baseline, BenchStats *measured) {
    double overhead_pct = ((double)measured->avg_ns - (double)baseline->avg_ns) / 
                          (double)baseline->avg_ns * 100.0;
    int64_t overhead_ns = (int64_t)measured->avg_ns - (int64_t)baseline->avg_ns;
    
    printf("%-35s: %+7.2f%% (%+7.3f us overhead)\n",
           name, overhead_pct, overhead_ns / 1000.0);
}

/* ==================== Test Payload (Simulated Work) ==================== */

typedef struct {
    int value;
    char buffer[64];
    double timestamp;
} WorkItem;

/* Simulate meaningful computational work */
static void do_computational_work(WorkItem *item) {
    /* Some non-trivial but consistent work */
    int sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += (i * item->value) % 997;  /* Prime modulo to avoid optimization */
    }
    item->value = sum;
    
    /* String manipulation */
    snprintf(item->buffer, sizeof(item->buffer), "Result: %d", sum);
    
    /* Floating point to prevent dead code elimination */
    item->timestamp = (double)sum * 1.234567;
}

/* ==================== TIER 1: Minimal Baseline ==================== */

/* Baseline: 3 bare function calls */
static void tier1_baseline_step1(WorkItem *item) {
    item->value += 10;
    do_computational_work(item);
}

static void tier1_baseline_step2(WorkItem *item) {
    item->value *= 2;
    do_computational_work(item);
}

static void tier1_baseline_step3(WorkItem *item) {
    item->value -= 5;
    do_computational_work(item);
}

static uint64_t tier1_baseline_execute(void) {
    WorkItem item = {42, {0}, 0.0};
    
    uint64_t start = get_time_ns();
    
    tier1_baseline_step1(&item);
    tier1_baseline_step2(&item);
    tier1_baseline_step3(&item);
    
    uint64_t end = get_time_ns();
    
    /* Prevent optimization */
    if (item.value < 0) printf("");
    
    return end - start;
}

/* EventChains: Full pattern with 0 middleware */
static EventResult tier1_event_step1(EventContext *ctx, void *user_data) {
    WorkItem *item = (WorkItem *)user_data;
    item->value += 10;
    do_computational_work(item);
    return event_result_success();
}

static EventResult tier1_event_step2(EventContext *ctx, void *user_data) {
    WorkItem *item = (WorkItem *)user_data;
    item->value *= 2;
    do_computational_work(item);
    return event_result_success();
}

static EventResult tier1_event_step3(EventContext *ctx, void *user_data) {
    WorkItem *item = (WorkItem *)user_data;
    item->value -= 5;
    do_computational_work(item);
    return event_result_success();
}

static uint64_t tier1_eventchains_execute(void) {
    WorkItem item = {42, {0}, 0.0};
    
    EventChain *chain = event_chain_create_strict();
    
    ChainableEvent *e1 = chainable_event_create(tier1_event_step1, &item, "Step1");
    ChainableEvent *e2 = chainable_event_create(tier1_event_step2, &item, "Step2");
    ChainableEvent *e3 = chainable_event_create(tier1_event_step3, &item, "Step3");
    
    event_chain_add_event(chain, e1);
    event_chain_add_event(chain, e2);
    event_chain_add_event(chain, e3);
    
    uint64_t start = get_time_ns();
    
    ChainResult result = event_chain_execute(chain);
    
    uint64_t end = get_time_ns();
    
    chain_result_destroy(&result);
    event_chain_destroy(chain);
    
    /* Prevent optimization */
    if (item.value < 0) printf("");
    
    return end - start;
}

static void run_tier1_benchmark(int iterations) {
    printf("\n|---------------------------------------------------------------|\n");
    printf("|  TIER 1: Minimal Baseline (Cost of Orchestration Framework)  |\n");
    printf("|---------------------------------------------------------------|\n\n");
    
    printf("Baseline: 3 bare function calls\n");
    printf("EventChains: Full pattern with 0 middleware\n");
    printf("Iterations: %d\n\n", iterations);
    
    uint64_t *baseline_samples = calloc(iterations, sizeof(uint64_t));
    uint64_t *eventchains_samples = calloc(iterations, sizeof(uint64_t));
    
    BenchStats baseline_stats, eventchains_stats;
    stats_init(&baseline_stats);
    stats_init(&eventchains_stats);
    
    /* Warm-up */
    for (int i = 0; i < 100; i++) {
        tier1_baseline_execute();
        tier1_eventchains_execute();
    }
    
    /* Collect baseline samples */
    for (int i = 0; i < iterations; i++) {
        uint64_t sample = tier1_baseline_execute();
        baseline_samples[i] = sample;
        stats_add_sample(&baseline_stats, sample);
    }
    
    /* Collect EventChains samples */
    for (int i = 0; i < iterations; i++) {
        uint64_t sample = tier1_eventchains_execute();
        eventchains_samples[i] = sample;
        stats_add_sample(&eventchains_stats, sample);
    }
    
    stats_finalize(&baseline_stats, baseline_samples);
    stats_finalize(&eventchains_stats, eventchains_samples);
    
    printf("Results:\n");
    printf("----------------------------------------------------------------\n");
    stats_print("Baseline (3 function calls)", &baseline_stats);
    stats_print("EventChains (0 middleware)", &eventchains_stats);
    printf("\n");
    stats_print_comparison("EventChains Overhead", &baseline_stats, &eventchains_stats);
    
    free(baseline_samples);
    free(eventchains_samples);
}

/* ==================== TIER 2: Feature-Parity Baseline ==================== */

typedef struct {
    char name[64];
    uint64_t duration_ns;
    bool success;
    char error_msg[128];
} ManualEventResult;

typedef struct {
    ManualEventResult *results;
    size_t count;
    size_t capacity;
} ManualChain;

static void manual_chain_init(ManualChain *chain) {
    chain->capacity = 8;
    chain->count = 0;
    chain->results = calloc(chain->capacity, sizeof(ManualEventResult));
}

static void manual_chain_destroy(ManualChain *chain) {
    free(chain->results);
    chain->results = NULL;
    chain->count = 0;
}

static void manual_chain_record(ManualChain *chain, const char *name, 
                                uint64_t duration, bool success, const char *error) {
    if (chain->count >= chain->capacity) return;
    
    ManualEventResult *result = &chain->results[chain->count++];
    strncpy(result->name, name, sizeof(result->name) - 1);
    result->duration_ns = duration;
    result->success = success;
    if (error) {
        strncpy(result->error_msg, error, sizeof(result->error_msg) - 1);
    } else {
        result->error_msg[0] = '\0';
    }
}

/* Baseline: Manual implementation with error handling, name tracking, cleanup */
static uint64_t tier2_baseline_execute(void) {
    WorkItem item = {42, {0}, 0.0};
    ManualChain chain;
    manual_chain_init(&chain);
    
    uint64_t start = get_time_ns();
    
    /* Step 1 with error handling and tracking */
    {
        uint64_t step_start = get_time_ns();
        item.value += 10;
        do_computational_work(&item);
        uint64_t step_end = get_time_ns();
        manual_chain_record(&chain, "Step1", step_end - step_start, true, NULL);
    }
    
    /* Step 2 with error handling and tracking */
    {
        uint64_t step_start = get_time_ns();
        item.value *= 2;
        do_computational_work(&item);
        uint64_t step_end = get_time_ns();
        manual_chain_record(&chain, "Step2", step_end - step_start, true, NULL);
    }
    
    /* Step 3 with error handling and tracking */
    {
        uint64_t step_start = get_time_ns();
        item.value -= 5;
        do_computational_work(&item);
        uint64_t step_end = get_time_ns();
        manual_chain_record(&chain, "Step3", step_end - step_start, true, NULL);
    }
    
    uint64_t end = get_time_ns();
    
    manual_chain_destroy(&chain);
    
    /* Prevent optimization */
    if (item.value < 0) printf("");
    
    return end - start;
}

/* EventChains: Same with 0 middleware */
static uint64_t tier2_eventchains_execute(void) {
    WorkItem item = {42, {0}, 0.0};
    
    EventChain *chain = event_chain_create_strict();
    
    ChainableEvent *e1 = chainable_event_create(tier1_event_step1, &item, "Step1");
    ChainableEvent *e2 = chainable_event_create(tier1_event_step2, &item, "Step2");
    ChainableEvent *e3 = chainable_event_create(tier1_event_step3, &item, "Step3");
    
    event_chain_add_event(chain, e1);
    event_chain_add_event(chain, e2);
    event_chain_add_event(chain, e3);
    
    uint64_t start = get_time_ns();
    
    ChainResult result = event_chain_execute(chain);
    
    uint64_t end = get_time_ns();
    
    chain_result_destroy(&result);
    event_chain_destroy(chain);
    
    /* Prevent optimization */
    if (item.value < 0) printf("");
    
    return end - start;
}

static void run_tier2_benchmark(int iterations) {
    printf("\n|---------------------------------------------------------------|\n");
    printf("|   TIER 2: Feature-Parity (Cost of Abstraction vs Manual)     |\n");
    printf("|---------------------------------------------------------------|\n\n");
    
    printf("Baseline: Manual error handling + name tracking + cleanup\n");
    printf("EventChains: Full pattern with 0 middleware\n");
    printf("Iterations: %d\n\n", iterations);
    
    uint64_t *baseline_samples = calloc(iterations, sizeof(uint64_t));
    uint64_t *eventchains_samples = calloc(iterations, sizeof(uint64_t));
    
    BenchStats baseline_stats, eventchains_stats;
    stats_init(&baseline_stats);
    stats_init(&eventchains_stats);
    
    /* Warm-up */
    for (int i = 0; i < 100; i++) {
        tier2_baseline_execute();
        tier2_eventchains_execute();
    }
    
    /* Collect samples */
    for (int i = 0; i < iterations; i++) {
        uint64_t sample = tier2_baseline_execute();
        baseline_samples[i] = sample;
        stats_add_sample(&baseline_stats, sample);
    }
    
    for (int i = 0; i < iterations; i++) {
        uint64_t sample = tier2_eventchains_execute();
        eventchains_samples[i] = sample;
        stats_add_sample(&eventchains_stats, sample);
    }
    
    stats_finalize(&baseline_stats, baseline_samples);
    stats_finalize(&eventchains_stats, eventchains_samples);
    
    printf("Results:\n");
    printf("----------------------------------------------------------------\n");
    stats_print("Baseline (manual equivalent)", &baseline_stats);
    stats_print("EventChains (0 middleware)", &eventchains_stats);
    printf("\n");
    stats_print_comparison("EventChains Overhead", &baseline_stats, &eventchains_stats);
    
    free(baseline_samples);
    free(eventchains_samples);
}

/* ==================== TIER 3: Middleware Scaling ==================== */

static EventResult noop_middleware(ChainableEvent *event, EventContext *context,
                                   MiddlewareNextFunc next, void *next_data, 
                                   void *user_data) {
    return next(event, context, next_data);
}

static uint64_t tier3_execute_with_middleware(int num_middleware) {
    WorkItem item = {42, {0}, 0.0};
    
    EventChain *chain = event_chain_create_strict();
    
    /* Add middleware layers */
    for (int i = 0; i < num_middleware; i++) {
        char name[64];
        snprintf(name, sizeof(name), "Middleware%d", i);
        EventMiddleware *mw = event_middleware_create(noop_middleware, NULL, name);
        event_chain_use_middleware(chain, mw);
    }
    
    /* Add events */
    ChainableEvent *e1 = chainable_event_create(tier1_event_step1, &item, "Step1");
    ChainableEvent *e2 = chainable_event_create(tier1_event_step2, &item, "Step2");
    ChainableEvent *e3 = chainable_event_create(tier1_event_step3, &item, "Step3");
    
    event_chain_add_event(chain, e1);
    event_chain_add_event(chain, e2);
    event_chain_add_event(chain, e3);
    
    uint64_t start = get_time_ns();
    
    ChainResult result = event_chain_execute(chain);
    
    uint64_t end = get_time_ns();
    
    chain_result_destroy(&result);
    event_chain_destroy(chain);
    
    /* Prevent optimization */
    if (item.value < 0) printf("");
    
    return end - start;
}

static void run_tier3_benchmark(int iterations) {
    printf("\n|---------------------------------------------------------------|\n");
    printf("|      TIER 3: Middleware Scaling (Cost per Middleware)        |\n");
    printf("|---------------------------------------------------------------|\n\n");
    
    printf("EventChains: 0, 1, 3, 5, 10 middleware layers\n");
    printf("Iterations: %d per configuration\n\n", iterations);
    
    int middleware_counts[] = {0, 1, 3, 5, 10};
    size_t num_configs = sizeof(middleware_counts) / sizeof(middleware_counts[0]);
    
    BenchStats stats_array[5];
    uint64_t *samples = calloc(iterations, sizeof(uint64_t));
    
    printf("Results:\n");
    printf("----------------------------------------------------------------\n");
    
    for (size_t config = 0; config < num_configs; config++) {
        int num_mw = middleware_counts[config];
        stats_init(&stats_array[config]);
        
        /* Warm-up */
        for (int i = 0; i < 50; i++) {
            tier3_execute_with_middleware(num_mw);
        }
        
        /* Collect samples */
        for (int i = 0; i < iterations; i++) {
            uint64_t sample = tier3_execute_with_middleware(num_mw);
            samples[i] = sample;
            stats_add_sample(&stats_array[config], sample);
        }
        
        stats_finalize(&stats_array[config], samples);
        
        char name[64];
        snprintf(name, sizeof(name), "%d middleware layer%s", 
                num_mw, num_mw == 1 ? "" : "s");
        stats_print(name, &stats_array[config]);
    }
    
    /* Analysis */
    printf("\nOverhead Analysis:\n");
    printf("----------------------------------------------------------------\n");
    
    for (size_t i = 1; i < num_configs; i++) {
        int64_t increment = (int64_t)stats_array[i].avg_ns - 
                           (int64_t)stats_array[i-1].avg_ns;
        int mw_added = middleware_counts[i] - middleware_counts[i-1];
        double per_mw = (double)increment / (double)mw_added;
        
        printf("%d -> %d middleware: %+7.3f us total, %.3f us per layer\n",
               middleware_counts[i-1], middleware_counts[i],
               increment / 1000.0, per_mw / 1000.0);
    }
    
    /* Amortized cost */
    if (num_configs > 2) {
        int64_t total_increment = (int64_t)stats_array[num_configs-1].avg_ns - 
                                  (int64_t)stats_array[0].avg_ns;
        int total_mw = middleware_counts[num_configs-1];
        double avg_per_mw = (double)total_increment / (double)total_mw;
        
        printf("\nAmortized cost per middleware layer: %.3f us\n", avg_per_mw / 1000.0);
    }
    
    free(samples);
}

/* ==================== TIER 4: Real-World Scenario ==================== */

/* Manual implementation with logging and timing */
typedef struct {
    uint64_t total_time;
    int event_count;
} ManualTimingContext;

static uint64_t tier4_baseline_execute(void) {
    WorkItem item = {42, {0}, 0.0};
    ManualTimingContext timing = {0, 0};
    
    uint64_t start = get_time_ns();
    
    /* Step 1 with logging and timing */
    {
        uint64_t step_start = get_time_ns();
        item.value += 10;
        do_computational_work(&item);
        uint64_t step_end = get_time_ns();
        
        uint64_t duration = step_end - step_start;
        timing.total_time += duration;
        timing.event_count++;
    }
    
    /* Step 2 with logging and timing */
    {
        uint64_t step_start = get_time_ns();
        item.value *= 2;
        do_computational_work(&item);
        uint64_t step_end = get_time_ns();
        
        uint64_t duration = step_end - step_start;
        timing.total_time += duration;
        timing.event_count++;
    }
    
    /* Step 3 with logging and timing */
    {
        uint64_t step_start = get_time_ns();
        item.value -= 5;
        do_computational_work(&item);
        uint64_t step_end = get_time_ns();
        
        uint64_t duration = step_end - step_start;
        timing.total_time += duration;
        timing.event_count++;
    }
    
    uint64_t end = get_time_ns();
    
    /* Prevent optimization */
    if (item.value < 0) printf("");
    if (timing.total_time > UINT64_MAX) printf("");
    
    return end - start;
}

/* EventChains with timing middleware */
typedef struct {
    uint64_t total_time;
    int event_count;
} TimingMiddlewareData;

static EventResult timing_middleware(ChainableEvent *event, EventContext *context,
                                     MiddlewareNextFunc next, void *next_data,
                                     void *user_data) {
    TimingMiddlewareData *timing = (TimingMiddlewareData *)user_data;
    
    uint64_t start = get_time_ns();
    EventResult result = next(event, context, next_data);
    uint64_t end = get_time_ns();
    
    timing->total_time += (end - start);
    timing->event_count++;
    
    return result;
}

static uint64_t tier4_eventchains_execute(void) {
    WorkItem item = {42, {0}, 0.0};
    TimingMiddlewareData timing = {0, 0};
    
    EventChain *chain = event_chain_create_strict();
    
    /* Add timing middleware */
    EventMiddleware *mw = event_middleware_create(timing_middleware, &timing, "Timing");
    event_chain_use_middleware(chain, mw);
    
    /* Add events */
    ChainableEvent *e1 = chainable_event_create(tier1_event_step1, &item, "Step1");
    ChainableEvent *e2 = chainable_event_create(tier1_event_step2, &item, "Step2");
    ChainableEvent *e3 = chainable_event_create(tier1_event_step3, &item, "Step3");
    
    event_chain_add_event(chain, e1);
    event_chain_add_event(chain, e2);
    event_chain_add_event(chain, e3);
    
    uint64_t start = get_time_ns();
    
    ChainResult result = event_chain_execute(chain);
    
    uint64_t end = get_time_ns();
    
    chain_result_destroy(&result);
    event_chain_destroy(chain);
    
    /* Prevent optimization */
    if (item.value < 0) printf("");
    if (timing.total_time > UINT64_MAX) printf("");
    
    return end - start;
}

static void run_tier4_benchmark(int iterations) {
    printf("\n|---------------------------------------------------------------|\n");
    printf("|  TIER 4: Real-World (Cost vs Manual Instrumentation)         |\n");
    printf("|---------------------------------------------------------------|\n\n");
    
    printf("Baseline: Manual logging + timing implementation\n");
    printf("EventChains: Logging + timing middleware\n");
    printf("Iterations: %d\n\n", iterations);
    
    uint64_t *baseline_samples = calloc(iterations, sizeof(uint64_t));
    uint64_t *eventchains_samples = calloc(iterations, sizeof(uint64_t));
    
    BenchStats baseline_stats, eventchains_stats;
    stats_init(&baseline_stats);
    stats_init(&eventchains_stats);
    
    /* Warm-up */
    for (int i = 0; i < 100; i++) {
        tier4_baseline_execute();
        tier4_eventchains_execute();
    }
    
    /* Collect samples */
    for (int i = 0; i < iterations; i++) {
        uint64_t sample = tier4_baseline_execute();
        baseline_samples[i] = sample;
        stats_add_sample(&baseline_stats, sample);
    }
    
    for (int i = 0; i < iterations; i++) {
        uint64_t sample = tier4_eventchains_execute();
        eventchains_samples[i] = sample;
        stats_add_sample(&eventchains_stats, sample);
    }
    
    stats_finalize(&baseline_stats, baseline_samples);
    stats_finalize(&eventchains_stats, eventchains_samples);
    
    printf("Results:\n");
    printf("----------------------------------------------------------------\n");
    stats_print("Baseline (manual instrumentation)", &baseline_stats);
    stats_print("EventChains (middleware-based)", &eventchains_stats);
    printf("\n");
    stats_print_comparison("EventChains Overhead", &baseline_stats, &eventchains_stats);
    
    free(baseline_samples);
    free(eventchains_samples);
}

/* ==================== Main Benchmark Runner ==================== */

int main(int argc, char *argv[]) {
    int iterations = 10000;
    
    if (argc > 1) {
        iterations = atoi(argv[1]);
        if (iterations < 100) iterations = 100;
        if (iterations > 100000) iterations = 100000;
    }
    
    printf("\n");
    printf("|---------------------------------------------------------------|\n");
    printf("|                                                               |\n");
    printf("|        EventChains Multi-Tier Benchmark Suite                |\n");
    printf("|         Measuring Abstraction Overhead at Scale              |\n");
    printf("|                                                               |\n");
    printf("|---------------------------------------------------------------|\n");
    
    printf("\n%s\n", event_chain_build_info());
    printf("\nBenchmark Configuration:\n");
    printf("  Iterations per test: %d\n", iterations);
    printf("  Timing precision: nanoseconds\n");
    printf("  Work per event: ~100 operations + string formatting\n");
    
    /* Run all tiers */
    run_tier1_benchmark(iterations);
    run_tier2_benchmark(iterations);
    run_tier3_benchmark(iterations);
    run_tier4_benchmark(iterations);
    
    /* Summary */
    printf("\n|---------------------------------------------------------------|\n");
    printf("|                     Benchmark Complete                       |\n");
    printf("|---------------------------------------------------------------|\n\n");
    
    printf("Key Findings:\n");
    printf("  Tier 1 shows raw orchestration framework overhead\n");
    printf("  Tier 2 shows abstraction cost vs feature-equivalent manual code\n");
    printf("  Tier 3 quantifies cost per middleware layer (amortized)\n");
    printf("  Tier 4 demonstrates real-world instrumentation scenarios\n\n");
    
    return 0;
}
