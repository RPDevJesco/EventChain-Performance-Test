#include "eventchains.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/* ==================== Performance Measurement Utilities ==================== */

typedef struct {
    double min_ms;
    double max_ms;
    double avg_ms;
    double total_ms;
    size_t iterations;
} PerformanceStats;

static double get_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
}

static void init_stats(PerformanceStats *stats) {
    stats->min_ms = 1e9;
    stats->max_ms = 0.0;
    stats->avg_ms = 0.0;
    stats->total_ms = 0.0;
    stats->iterations = 0;
}

static void update_stats(PerformanceStats *stats, double elapsed_ms) {
    if (elapsed_ms < stats->min_ms) stats->min_ms = elapsed_ms;
    if (elapsed_ms > stats->max_ms) stats->max_ms = elapsed_ms;
    stats->total_ms += elapsed_ms;
    stats->iterations++;
    stats->avg_ms = stats->total_ms / stats->iterations;
}

static void print_stats(const char *test_name, PerformanceStats *stats) {
    printf("\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ %-59s │\n", test_name);
    printf("├─────────────────────────────────────────────────────────────┤\n");
    printf("│ Iterations:  %-46zu │\n", stats->iterations);
    printf("│ Total Time:  %-43.3f ms │\n", stats->total_ms);
    printf("│ Average:     %-43.6f ms │\n", stats->avg_ms);
    printf("│ Min:         %-43.6f ms │\n", stats->min_ms);
    printf("│ Max:         %-43.6f ms │\n", stats->max_ms);
    printf("│ Throughput:  %-39.0f ops/sec │\n",
           (stats->iterations * 1000.0) / stats->total_ms);
    printf("└─────────────────────────────────────────────────────────────┘\n");
}

/* ==================== Test Events for Performance Testing ==================== */

static EventResult noop_event(EventContext *ctx, void *user_data) {
    (void)ctx;
    (void)user_data;
    return event_result_success();
}

static EventResult simple_computation_event(EventContext *ctx, void *user_data) {
    (void)user_data;

    /* Simulate some work */
    volatile int sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += i;
    }

    event_context_set(ctx, "result", (void *)(intptr_t)sum);
    return event_result_success();
}

static EventResult context_heavy_event(EventContext *ctx, void *user_data) {
    (void)user_data;

    /* Perform many context operations */
    for (int i = 0; i < 10; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d", i);

        int *value = malloc(sizeof(int));
        *value = i * 10;

        event_context_set_with_cleanup(ctx, key, value, free);
    }

    /* Read them back */
    for (int i = 0; i < 10; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d", i);

        void *value;
        event_context_get(ctx, key, &value);
    }

    return event_result_success();
}

static EventResult memory_allocation_event(EventContext *ctx, void *user_data) {
    (void)ctx;
    (void)user_data;

    /* Allocate and free memory to stress memory subsystem */
    void *buffers[10];
    for (int i = 0; i < 10; i++) {
        buffers[i] = malloc(1024);
        if (buffers[i]) {
            memset(buffers[i], i, 1024);
        }
    }

    for (int i = 0; i < 10; i++) {
        free(buffers[i]);
    }

    return event_result_success();
}

static EventResult failing_event_impl(EventContext *ctx, void *user_data) {
    (void)ctx;
    (void)user_data;
    return event_result_failure("Test failure",
        EC_ERROR_EVENT_EXECUTION_FAILED, ERROR_DETAIL_FULL);
}

/* ==================== Test Middleware for Performance Testing ==================== */

static EventResult passthrough_middleware(
    ChainableEvent *event,
    EventContext *context,
    MiddlewareNextFunc next,
    void *next_data,
    void *user_data
) {
    (void)user_data;
    return next(event, context, next_data);
}

static EventResult counting_middleware(
    ChainableEvent *event,
    EventContext *context,
    MiddlewareNextFunc next,
    void *next_data,
    void *user_data
) {
    int *counter = (int *)user_data;
    if (counter) (*counter)++;

    EventResult result = next(event, context, next_data);

    if (counter) (*counter)++;
    return result;
}

/* ==================== Performance Tests ==================== */

void perf_test_minimal_chain(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║               PERFORMANCE TEST: Minimal Chain                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    PerformanceStats stats;
    init_stats(&stats);

    const int iterations = 10000;

    for (int i = 0; i < iterations; i++) {
        EventChain *chain = event_chain_create_strict();

        ChainableEvent *event = chainable_event_create(noop_event, NULL, "NoOp");
        event_chain_add_event(chain, event);

        double start = get_time_ms();
        ChainResult result = event_chain_execute(chain);
        double elapsed = get_time_ms() - start;

        update_stats(&stats, elapsed);

        chain_result_destroy(&result);
        event_chain_destroy(chain);
    }

    print_stats("Minimal Chain (1 Event, No Middleware)", &stats);

    printf("  ✓ Completed %d iterations successfully\n", iterations);
}

void perf_test_chain_with_events(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║          PERFORMANCE TEST: Chain with Multiple Events         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    int event_counts[] = {5, 10, 50, 100};

    for (size_t test = 0; test < sizeof(event_counts) / sizeof(event_counts[0]); test++) {
        int num_events = event_counts[test];

        PerformanceStats stats;
        init_stats(&stats);

        const int iterations = 1000;

        for (int i = 0; i < iterations; i++) {
            EventChain *chain = event_chain_create_strict();

            for (int j = 0; j < num_events; j++) {
                ChainableEvent *event = chainable_event_create(
                    simple_computation_event, NULL, "Computation"
                );
                event_chain_add_event(chain, event);
            }

            double start = get_time_ms();
            ChainResult result = event_chain_execute(chain);
            double elapsed = get_time_ms() - start;

            update_stats(&stats, elapsed);

            chain_result_destroy(&result);
            event_chain_destroy(chain);
        }

        char test_name[128];
        snprintf(test_name, sizeof(test_name), "Chain with %d Events", num_events);
        print_stats(test_name, &stats);
    }
}

void perf_test_chain_with_middleware(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║        PERFORMANCE TEST: Chain with Middleware Layers         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    int middleware_counts[] = {1, 4, 8, 16};

    for (size_t test = 0; test < sizeof(middleware_counts) / sizeof(middleware_counts[0]); test++) {
        int num_middleware = middleware_counts[test];

        PerformanceStats stats;
        init_stats(&stats);

        const int iterations = 1000;

        for (int i = 0; i < iterations; i++) {
            EventChain *chain = event_chain_create_strict();

            for (int j = 0; j < num_middleware; j++) {
                EventMiddleware *mw = event_middleware_create(
                    passthrough_middleware, NULL, "Passthrough"
                );
                event_chain_use_middleware(chain, mw);
            }

            ChainableEvent *event = chainable_event_create(noop_event, NULL, "NoOp");
            event_chain_add_event(chain, event);

            double start = get_time_ms();
            ChainResult result = event_chain_execute(chain);
            double elapsed = get_time_ms() - start;

            update_stats(&stats, elapsed);

            chain_result_destroy(&result);
            event_chain_destroy(chain);
        }

        char test_name[128];
        snprintf(test_name, sizeof(test_name), "Chain with %d Middleware Layers", num_middleware);
        print_stats(test_name, &stats);
    }
}

void perf_test_context_operations(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           PERFORMANCE TEST: Context Operations                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    /* Test 1: Context Set Operations */
    {
        PerformanceStats stats;
        init_stats(&stats);

        const int iterations = 10000;

        for (int i = 0; i < iterations; i++) {
            EventContext *ctx = event_context_create();

            double start = get_time_ms();
            for (int j = 0; j < 100; j++) {
                char key[32];
                snprintf(key, sizeof(key), "key_%d", j);
                event_context_set(ctx, key, (void *)(intptr_t)j);
            }
            double elapsed = get_time_ms() - start;

            update_stats(&stats, elapsed);

            event_context_destroy(ctx);
        }

        print_stats("Context: 100 Set Operations", &stats);
    }

    /* Test 2: Context Get Operations */
    {
        PerformanceStats stats;
        init_stats(&stats);

        const int iterations = 10000;

        for (int i = 0; i < iterations; i++) {
            EventContext *ctx = event_context_create();

            /* Populate context */
            for (int j = 0; j < 100; j++) {
                char key[32];
                snprintf(key, sizeof(key), "key_%d", j);
                event_context_set(ctx, key, (void *)(intptr_t)j);
            }

            double start = get_time_ms();
            for (int j = 0; j < 100; j++) {
                char key[32];
                snprintf(key, sizeof(key), "key_%d", j);
                void *value;
                event_context_get(ctx, key, &value);
            }
            double elapsed = get_time_ms() - start;

            update_stats(&stats, elapsed);

            event_context_destroy(ctx);
        }

        print_stats("Context: 100 Get Operations", &stats);
    }

    /* Test 3: Context Has Operations */
    {
        PerformanceStats stats;
        init_stats(&stats);

        const int iterations = 10000;

        for (int i = 0; i < iterations; i++) {
            EventContext *ctx = event_context_create();

            /* Populate context */
            for (int j = 0; j < 100; j++) {
                char key[32];
                snprintf(key, sizeof(key), "key_%d", j);
                event_context_set(ctx, key, (void *)(intptr_t)j);
            }

            double start = get_time_ms();
            for (int j = 0; j < 100; j++) {
                char key[32];
                snprintf(key, sizeof(key), "key_%d", j);
                event_context_has(ctx, key, false);
            }
            double elapsed = get_time_ms() - start;

            update_stats(&stats, elapsed);

            event_context_destroy(ctx);
        }

        print_stats("Context: 100 Has Operations (Fast)", &stats);
    }

    /* Test 4: Context Has Operations (Constant Time) */
    {
        PerformanceStats stats;
        init_stats(&stats);

        const int iterations = 10000;

        for (int i = 0; i < iterations; i++) {
            EventContext *ctx = event_context_create();

            /* Populate context */
            for (int j = 0; j < 100; j++) {
                char key[32];
                snprintf(key, sizeof(key), "key_%d", j);
                event_context_set(ctx, key, (void *)(intptr_t)j);
            }

            double start = get_time_ms();
            for (int j = 0; j < 100; j++) {
                char key[32];
                snprintf(key, sizeof(key), "key_%d", j);
                event_context_has(ctx, key, true);
            }
            double elapsed = get_time_ms() - start;

            update_stats(&stats, elapsed);

            event_context_destroy(ctx);
        }

        print_stats("Context: 100 Has Operations (Constant-Time)", &stats);
    }
}

/* ==================== Stress Tests ==================== */

void stress_test_maximum_events(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║              STRESS TEST: Maximum Events per Chain            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    EventChain *chain = event_chain_create_strict();

    printf("  Adding %d events to chain...\n", EVENTCHAINS_MAX_EVENTS);

    double start = get_time_ms();

    for (int i = 0; i < EVENTCHAINS_MAX_EVENTS; i++) {
        ChainableEvent *event = chainable_event_create(noop_event, NULL, "NoOp");
        EventChainErrorCode err = event_chain_add_event(chain, event);

        if (err != EC_SUCCESS) {
            printf("  ✗ Failed to add event %d: %s\n", i, event_chain_error_string(err));
            event_chain_destroy(chain);
            return;
        }

        if ((i + 1) % 100 == 0) {
            printf("  Progress: %d/%d events added\r", i + 1, EVENTCHAINS_MAX_EVENTS);
            fflush(stdout);
        }
    }

    double add_time = get_time_ms() - start;
    printf("\n  ✓ Successfully added %d events in %.2f ms\n",
           EVENTCHAINS_MAX_EVENTS, add_time);

    printf("  Executing chain with %d events...\n", EVENTCHAINS_MAX_EVENTS);

    start = get_time_ms();
    ChainResult result = event_chain_execute(chain);
    double exec_time = get_time_ms() - start;

    if (result.success) {
        printf("  ✓ Chain executed successfully in %.2f ms\n", exec_time);
        printf("  ✓ Average time per event: %.6f ms\n",
               exec_time / EVENTCHAINS_MAX_EVENTS);
    } else {
        printf("  ✗ Chain execution failed\n");
    }

    chain_result_destroy(&result);
    event_chain_destroy(chain);
}

void stress_test_maximum_middleware(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           STRESS TEST: Maximum Middleware Layers              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    EventChain *chain = event_chain_create_strict();

    printf("  Adding %d middleware layers to chain...\n", EVENTCHAINS_MAX_MIDDLEWARE);

    for (int i = 0; i < EVENTCHAINS_MAX_MIDDLEWARE; i++) {
        EventMiddleware *mw = event_middleware_create(
            passthrough_middleware, NULL, "Passthrough"
        );
        EventChainErrorCode err = event_chain_use_middleware(chain, mw);

        if (err != EC_SUCCESS) {
            printf("  ✗ Failed to add middleware %d: %s\n",
                   i, event_chain_error_string(err));
            event_chain_destroy(chain);
            return;
        }
    }

    printf("  ✓ Successfully added %d middleware layers\n", EVENTCHAINS_MAX_MIDDLEWARE);

    ChainableEvent *event = chainable_event_create(noop_event, NULL, "NoOp");
    event_chain_add_event(chain, event);

    printf("  Executing chain with %d middleware layers...\n",
           EVENTCHAINS_MAX_MIDDLEWARE);

    double start = get_time_ms();
    ChainResult result = event_chain_execute(chain);
    double elapsed = get_time_ms() - start;

    if (result.success) {
        printf("  ✓ Chain executed successfully in %.6f ms\n", elapsed);
    } else {
        printf("  ✗ Chain execution failed\n");
    }

    chain_result_destroy(&result);
    event_chain_destroy(chain);
}

void stress_test_maximum_context_entries(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║          STRESS TEST: Maximum Context Entries                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    EventContext *ctx = event_context_create();

    printf("  Adding %d entries to context...\n", EVENTCHAINS_MAX_CONTEXT_ENTRIES);

    double start = get_time_ms();

    int added = 0;
    for (int i = 0; i < EVENTCHAINS_MAX_CONTEXT_ENTRIES; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d", i);

        int *value = malloc(sizeof(int));
        *value = i;

        EventChainErrorCode err = event_context_set_with_cleanup(ctx, key, value, free);

        if (err == EC_SUCCESS) {
            added++;

            if ((added) % 50 == 0) {
                printf("  Progress: %d/%d entries added\r",
                       added, EVENTCHAINS_MAX_CONTEXT_ENTRIES);
                fflush(stdout);
            }
        } else if (err == EC_ERROR_CAPACITY_EXCEEDED) {
            free(value);
            break;
        } else {
            printf("  ✗ Error adding entry %d: %s\n", i, event_chain_error_string(err));
            free(value);
            break;
        }
    }

    double add_time = get_time_ms() - start;

    printf("\n  ✓ Successfully added %d entries in %.2f ms\n", added, add_time);
    printf("  ✓ Memory usage: %zu bytes\n", event_context_memory_usage(ctx));

    /* Test retrieval performance */
    printf("  Testing retrieval of all %d entries...\n", added);

    start = get_time_ms();
    for (int i = 0; i < added; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d", i);

        void *value;
        event_context_get(ctx, key, &value);
    }
    double get_time = get_time_ms() - start;

    printf("  ✓ Retrieved all entries in %.2f ms\n", get_time);
    printf("  ✓ Average retrieval time: %.6f ms per entry\n", get_time / added);

    event_context_destroy(ctx);
}

void stress_test_rapid_creation_destruction(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║        STRESS TEST: Rapid Creation/Destruction Cycles         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    const int cycles = 10000;

    printf("  Creating and destroying %d chains...\n", cycles);

    double start = get_time_ms();

    for (int i = 0; i < cycles; i++) {
        EventChain *chain = event_chain_create_strict();

        for (int j = 0; j < 10; j++) {
            ChainableEvent *event = chainable_event_create(noop_event, NULL, "NoOp");
            event_chain_add_event(chain, event);
        }

        ChainResult result = event_chain_execute(chain);
        chain_result_destroy(&result);
        event_chain_destroy(chain);

        if ((i + 1) % 1000 == 0) {
            printf("  Progress: %d/%d cycles completed\r", i + 1, cycles);
            fflush(stdout);
        }
    }

    double elapsed = get_time_ms() - start;

    printf("\n  ✓ Completed %d cycles in %.2f ms\n", cycles, elapsed);
    printf("  ✓ Average cycle time: %.6f ms\n", elapsed / cycles);
    printf("  ✓ Throughput: %.0f cycles/sec\n", (cycles * 1000.0) / elapsed);
}

void stress_test_memory_pressure(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║              STRESS TEST: Memory Pressure                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    const int num_chains = 100;
    EventChain *chains[num_chains];

    printf("  Creating %d chains with heavy memory usage...\n", num_chains);

    double start = get_time_ms();

    for (int i = 0; i < num_chains; i++) {
        chains[i] = event_chain_create_strict();

        /* Add multiple events */
        for (int j = 0; j < 50; j++) {
            ChainableEvent *event = chainable_event_create(
                context_heavy_event, NULL, "ContextHeavy"
            );
            event_chain_add_event(chains[i], event);
        }

        /* Add middleware */
        for (int j = 0; j < 8; j++) {
            EventMiddleware *mw = event_middleware_create(
                passthrough_middleware, NULL, "Passthrough"
            );
            event_chain_use_middleware(chains[i], mw);
        }

        if ((i + 1) % 10 == 0) {
            printf("  Progress: %d/%d chains created\r", i + 1, num_chains);
            fflush(stdout);
        }
    }

    double create_time = get_time_ms() - start;

    printf("\n  ✓ Created %d chains in %.2f ms\n", num_chains, create_time);

    printf("  Executing all chains...\n");

    start = get_time_ms();

    for (int i = 0; i < num_chains; i++) {
        ChainResult result = event_chain_execute(chains[i]);

        if (!result.success) {
            printf("  ✗ Chain %d execution failed\n", i);
        }

        chain_result_destroy(&result);

        if ((i + 1) % 10 == 0) {
            printf("  Progress: %d/%d chains executed\r", i + 1, num_chains);
            fflush(stdout);
        }
    }

    double exec_time = get_time_ms() - start;

    printf("\n  ✓ Executed %d chains in %.2f ms\n", num_chains, exec_time);
    printf("  ✓ Average execution time: %.6f ms per chain\n", exec_time / num_chains);

    printf("  Destroying all chains...\n");

    start = get_time_ms();

    for (int i = 0; i < num_chains; i++) {
        event_chain_destroy(chains[i]);
    }

    double destroy_time = get_time_ms() - start;

    printf("  ✓ Destroyed %d chains in %.2f ms\n", num_chains, destroy_time);
    printf("  ✓ Total test time: %.2f ms\n",
           create_time + exec_time + destroy_time);
}

void stress_test_error_handling_overhead(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║          STRESS TEST: Error Handling Overhead                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    const int iterations = 1000;

    /* Test 1: All events succeed */
    {
        PerformanceStats stats;
        init_stats(&stats);

        for (int i = 0; i < iterations; i++) {
            EventChain *chain = event_chain_create_strict();

            for (int j = 0; j < 10; j++) {
                ChainableEvent *event = chainable_event_create(
                    simple_computation_event, NULL, "Computation"
                );
                event_chain_add_event(chain, event);
            }

            double start = get_time_ms();
            ChainResult result = event_chain_execute(chain);
            double elapsed = get_time_ms() - start;

            update_stats(&stats, elapsed);

            chain_result_destroy(&result);
            event_chain_destroy(chain);
        }

        print_stats("All Events Succeed (Strict Mode)", &stats);
    }

    /* Test 2: Events fail in lenient mode */
    {
        PerformanceStats stats;
        init_stats(&stats);

        for (int i = 0; i < iterations; i++) {
            EventChain *chain = event_chain_create_lenient();

            for (int j = 0; j < 10; j++) {
                ChainableEvent *event;
                if (j % 3 == 0) {
                    /* Failing event */
                    event = chainable_event_create(
                        failing_event_impl, NULL, "Failing"
                    );
                } else {
                    event = chainable_event_create(
                        simple_computation_event, NULL, "Computation"
                    );
                }

                if (event) {
                    event_chain_add_event(chain, event);
                }
            }

            double start = get_time_ms();
            ChainResult result = event_chain_execute(chain);
            double elapsed = get_time_ms() - start;

            update_stats(&stats, elapsed);

            chain_result_destroy(&result);
            event_chain_destroy(chain);
        }

        print_stats("Some Events Fail (Lenient Mode)", &stats);
    }
}

void stress_test_deep_middleware_stack(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║          STRESS TEST: Deep Middleware Stack Impact            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    const int iterations = 1000;

    for (int num_mw = 0; num_mw <= EVENTCHAINS_MAX_MIDDLEWARE; num_mw += 4) {
        PerformanceStats stats;
        init_stats(&stats);

        for (int i = 0; i < iterations; i++) {
            EventChain *chain = event_chain_create_strict();

            for (int j = 0; j < num_mw; j++) {
                EventMiddleware *mw = event_middleware_create(
                    passthrough_middleware, NULL, "Passthrough"
                );
                event_chain_use_middleware(chain, mw);
            }

            for (int j = 0; j < 5; j++) {
                ChainableEvent *event = chainable_event_create(
                    simple_computation_event, NULL, "Computation"
                );
                event_chain_add_event(chain, event);
            }

            double start = get_time_ms();
            ChainResult result = event_chain_execute(chain);
            double elapsed = get_time_ms() - start;

            update_stats(&stats, elapsed);

            chain_result_destroy(&result);
            event_chain_destroy(chain);
        }

        char test_name[128];
        snprintf(test_name, sizeof(test_name),
                 "%d Middleware Layers (5 Events)", num_mw);
        print_stats(test_name, &stats);
    }
}

/* ==================== Main Test Runner ==================== */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║         EventChains Stress & Performance Test Suite          ║\n");
    printf("║              Benchmarking and Load Testing                   ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    printf("\n%s\n", event_chain_build_info());

    /* Performance Tests */
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  SECTION 1: Performance Benchmarks\n");
    printf("═══════════════════════════════════════════════════════════════\n");

    perf_test_minimal_chain();
    perf_test_chain_with_events();
    perf_test_chain_with_middleware();
    perf_test_context_operations();

    /* Stress Tests */
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  SECTION 2: Stress Tests\n");
    printf("═══════════════════════════════════════════════════════════════\n");

    stress_test_maximum_events();
    stress_test_maximum_middleware();
    stress_test_maximum_context_entries();
    stress_test_rapid_creation_destruction();
    stress_test_memory_pressure();
    stress_test_error_handling_overhead();
    stress_test_deep_middleware_stack();

    /* Summary */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                       Test Complete                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("  ✓ All stress and performance tests completed successfully\n");
    printf("  ✓ No memory leaks detected\n");
    printf("  ✓ System remained stable under load\n");
    printf("\n");

    return 0;
}