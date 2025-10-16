#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 600

#include "eventchains.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

/* Fallback for systems without CLOCK_MONOTONIC */
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0
#warning "CLOCK_MONOTONIC not available, using fallback"
#endif

/* ==================== Graph Data Structures ==================== */

typedef struct Edge {
    int to;
    int weight;
    struct Edge *next;
} Edge;

typedef struct Graph {
    int num_vertices;
    int num_edges;
    Edge **adj_list;
} Graph;

typedef struct {
    int vertex;
    int distance;
} HeapNode;

typedef struct {
    HeapNode *nodes;
    int *positions;  /* vertex -> heap position */
    int size;
    int capacity;
} MinHeap;

/* ==================== Performance Profiling ==================== */

typedef struct {
    uint64_t execution_time_ns;
    size_t memory_allocated;
    size_t num_allocations;
    size_t context_lookups;
    size_t middleware_calls;
    size_t event_wrapping_overhead;
} ProfileData;

static ProfileData global_profile;

#if defined(_WIN32) || defined(_WIN64)
/* Windows fallback */
#include <windows.h>
static inline uint64_t get_time_ns(void) {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (uint64_t)((counter.QuadPart * 1000000000ULL) / frequency.QuadPart);
}
#elif defined(__MACH__) && defined(__APPLE__)
/* macOS fallback */
#include <mach/mach_time.h>
static inline uint64_t get_time_ns(void) {
    static mach_timebase_info_data_t timebase;
    if (timebase.denom == 0) {
        mach_timebase_info(&timebase);
    }
    return (mach_absolute_time() * timebase.numer) / timebase.denom;
}
#elif defined(CLOCK_MONOTONIC)
/* POSIX systems with clock_gettime */
static inline uint64_t get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}
#else
/* Fallback using standard clock() - much lower precision */
#warning "Using low-precision clock() for timing - results may be inaccurate"
static inline uint64_t get_time_ns(void) {
    return (uint64_t)(clock() * 1000000000ULL / CLOCKS_PER_SEC);
}
#endif

static void profile_reset(ProfileData *profile) {
    memset(profile, 0, sizeof(ProfileData));
}

static void profile_alloc(size_t bytes) {
    global_profile.memory_allocated += bytes;
    global_profile.num_allocations++;
}

/* ==================== Graph Functions ==================== */

Graph *graph_create(int num_vertices) {
    Graph *g = malloc(sizeof(Graph));
    profile_alloc(sizeof(Graph));
    
    g->num_vertices = num_vertices;
    g->num_edges = 0;
    g->adj_list = calloc(num_vertices, sizeof(Edge *));
    profile_alloc(num_vertices * sizeof(Edge *));
    
    return g;
}

void graph_add_edge(Graph *g, int from, int to, int weight) {
    Edge *edge = malloc(sizeof(Edge));
    profile_alloc(sizeof(Edge));
    
    edge->to = to;
    edge->weight = weight;
    edge->next = g->adj_list[from];
    g->adj_list[from] = edge;
    g->num_edges++;
}

void graph_destroy(Graph *g) {
    for (int i = 0; i < g->num_vertices; i++) {
        Edge *edge = g->adj_list[i];
        while (edge) {
            Edge *temp = edge;
            edge = edge->next;
            free(temp);
        }
    }
    free(g->adj_list);
    free(g);
}

Graph *graph_generate_random(int num_vertices, int num_edges, unsigned int seed) {
    srand(seed);
    Graph *g = graph_create(num_vertices);
    
    for (int i = 0; i < num_edges; i++) {
        int from = rand() % num_vertices;
        int to = rand() % num_vertices;
        int weight = (rand() % 100) + 1;  /* Weight between 1 and 100 */
        
        if (from != to) {
            graph_add_edge(g, from, to, weight);
        }
    }
    
    return g;
}

/* ==================== Min Heap Implementation ==================== */

MinHeap *heap_create(int capacity) {
    MinHeap *heap = malloc(sizeof(MinHeap));
    profile_alloc(sizeof(MinHeap));
    
    heap->nodes = malloc(sizeof(HeapNode) * capacity);
    heap->positions = malloc(sizeof(int) * capacity);
    profile_alloc(sizeof(HeapNode) * capacity + sizeof(int) * capacity);
    
    heap->size = 0;
    heap->capacity = capacity;
    
    for (int i = 0; i < capacity; i++) {
        heap->positions[i] = -1;
    }
    
    return heap;
}

void heap_destroy(MinHeap *heap) {
    free(heap->nodes);
    free(heap->positions);
    free(heap);
}

static void heap_swap(MinHeap *heap, int i, int j) {
    HeapNode temp = heap->nodes[i];
    heap->nodes[i] = heap->nodes[j];
    heap->nodes[j] = temp;
    
    heap->positions[heap->nodes[i].vertex] = i;
    heap->positions[heap->nodes[j].vertex] = j;
}

static void heap_heapify_up(MinHeap *heap, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (heap->nodes[idx].distance >= heap->nodes[parent].distance) {
            break;
        }
        heap_swap(heap, idx, parent);
        idx = parent;
    }
}

static void heap_heapify_down(MinHeap *heap, int idx) {
    while (1) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int smallest = idx;
        
        if (left < heap->size && heap->nodes[left].distance < heap->nodes[smallest].distance) {
            smallest = left;
        }
        if (right < heap->size && heap->nodes[right].distance < heap->nodes[smallest].distance) {
            smallest = right;
        }
        
        if (smallest == idx) {
            break;
        }
        
        heap_swap(heap, idx, smallest);
        idx = smallest;
    }
}

void heap_insert(MinHeap *heap, int vertex, int distance) {
    if (heap->size >= heap->capacity) return;
    
    int idx = heap->size++;
    heap->nodes[idx].vertex = vertex;
    heap->nodes[idx].distance = distance;
    heap->positions[vertex] = idx;
    
    heap_heapify_up(heap, idx);
}

HeapNode heap_extract_min(MinHeap *heap) {
    HeapNode min = heap->nodes[0];
    
    heap->positions[min.vertex] = -1;
    heap->size--;
    
    if (heap->size > 0) {
        heap->nodes[0] = heap->nodes[heap->size];
        heap->positions[heap->nodes[0].vertex] = 0;
        heap_heapify_down(heap, 0);
    }
    
    return min;
}

void heap_decrease_key(MinHeap *heap, int vertex, int new_distance) {
    int idx = heap->positions[vertex];
    if (idx == -1) return;
    
    heap->nodes[idx].distance = new_distance;
    heap_heapify_up(heap, idx);
}

bool heap_is_empty(MinHeap *heap) {
    return heap->size == 0;
}

/* ==================== Traditional Dijkstra Implementation ==================== */

typedef struct {
    int *distances;
    int *predecessors;
    bool success;
} DijkstraResult;

DijkstraResult dijkstra_traditional(Graph *g, int source, bool verbose) {
    DijkstraResult result;
    result.distances = malloc(sizeof(int) * g->num_vertices);
    result.predecessors = malloc(sizeof(int) * g->num_vertices);
    profile_alloc(sizeof(int) * g->num_vertices * 2);
    
    if (verbose) printf("[Traditional] Initializing distances\n");
    
    /* Initialize distances */
    for (int i = 0; i < g->num_vertices; i++) {
        result.distances[i] = INT_MAX;
        result.predecessors[i] = -1;
    }
    result.distances[source] = 0;
    
    if (verbose) printf("[Traditional] Creating priority queue\n");
    
    /* Create min heap */
    MinHeap *heap = heap_create(g->num_vertices);
    heap_insert(heap, source, 0);
    
    if (verbose) printf("[Traditional] Processing vertices\n");
    
    int vertices_processed = 0;
    
    /* Main loop */
    while (!heap_is_empty(heap)) {
        HeapNode current = heap_extract_min(heap);
        int u = current.vertex;
        vertices_processed++;
        
        if (current.distance > result.distances[u]) {
            continue;
        }
        
        /* Relax edges */
        Edge *edge = g->adj_list[u];
        while (edge) {
            int v = edge->to;
            int alt = result.distances[u] + edge->weight;
            
            if (alt < result.distances[v]) {
                result.distances[v] = alt;
                result.predecessors[v] = u;
                
                if (heap->positions[v] == -1) {
                    heap_insert(heap, v, alt);
                } else {
                    heap_decrease_key(heap, v, alt);
                }
            }
            
            edge = edge->next;
        }
    }
    
    if (verbose) printf("[Traditional] Processed %d vertices\n", vertices_processed);
    
    heap_destroy(heap);
    result.success = true;
    
    return result;
}

/* ==================== EventChains Dijkstra Implementation ==================== */

/* Context keys */
#define CTX_GRAPH "graph"
#define CTX_SOURCE "source"
#define CTX_DISTANCES "distances"
#define CTX_PREDECESSORS "predecessors"
#define CTX_HEAP "heap"
#define CTX_VERTICES_PROCESSED "vertices_processed"
#define CTX_VERBOSE "verbose"

/* Event 1: Initialize distances */
EventResult event_initialize(EventContext *context, void *user_data) {
    global_profile.context_lookups += 3;
    
    void *graph_ptr, *source_ptr, *verbose_ptr;
    event_context_get(context, CTX_GRAPH, &graph_ptr);
    event_context_get(context, CTX_SOURCE, &source_ptr);
    event_context_get(context, CTX_VERBOSE, &verbose_ptr);

    Graph *g = (Graph *)graph_ptr;
    int *source = (int *)source_ptr;
    bool *verbose = (bool *)verbose_ptr;

    if (!g || !source) {
        return event_result_failure("Missing graph or source", EC_ERROR_NULL_POINTER, ERROR_DETAIL_FULL);
    }

    if (verbose && *verbose) {
        printf("[EventChain] Event: Initialize distances\n");
    }

    int *distances = malloc(sizeof(int) * g->num_vertices);
    int *predecessors = malloc(sizeof(int) * g->num_vertices);
    profile_alloc(sizeof(int) * g->num_vertices * 2);

    for (int i = 0; i < g->num_vertices; i++) {
        distances[i] = INT_MAX;
        predecessors[i] = -1;
    }
    distances[*source] = 0;

    event_context_set(context, CTX_DISTANCES, distances);
    event_context_set(context, CTX_PREDECESSORS, predecessors);

    return event_result_success();
}

/* Event 2: Create priority queue */
EventResult event_create_heap(EventContext *context, void *user_data) {
    global_profile.context_lookups += 3;

    void *graph_ptr, *source_ptr, *verbose_ptr;
    event_context_get(context, CTX_GRAPH, &graph_ptr);
    event_context_get(context, CTX_SOURCE, &source_ptr);
    event_context_get(context, CTX_VERBOSE, &verbose_ptr);

    Graph *g = (Graph *)graph_ptr;
    int *source = (int *)source_ptr;
    bool *verbose = (bool *)verbose_ptr;

    if (!g || !source) {
        return event_result_failure("Missing graph or source", EC_ERROR_NULL_POINTER, ERROR_DETAIL_FULL);
    }

    if (verbose && *verbose) {
        printf("[EventChain] Event: Create priority queue\n");
    }

    MinHeap *heap = heap_create(g->num_vertices);
    heap_insert(heap, *source, 0);

    event_context_set(context, CTX_HEAP, heap);

    int *vertices_processed = malloc(sizeof(int));
    profile_alloc(sizeof(int));
    *vertices_processed = 0;
    event_context_set(context, CTX_VERTICES_PROCESSED, vertices_processed);

    return event_result_success();
}

/* Event 3: Process vertices */
EventResult event_process_vertices(EventContext *context, void *user_data) {
    global_profile.context_lookups += 5;

    void *graph_ptr, *heap_ptr, *distances_ptr, *predecessors_ptr, *vertices_processed_ptr, *verbose_ptr;
    event_context_get(context, CTX_GRAPH, &graph_ptr);
    event_context_get(context, CTX_HEAP, &heap_ptr);
    event_context_get(context, CTX_DISTANCES, &distances_ptr);
    event_context_get(context, CTX_PREDECESSORS, &predecessors_ptr);
    event_context_get(context, CTX_VERTICES_PROCESSED, &vertices_processed_ptr);
    event_context_get(context, CTX_VERBOSE, &verbose_ptr);

    Graph *g = (Graph *)graph_ptr;
    MinHeap *heap = (MinHeap *)heap_ptr;
    int *distances = (int *)distances_ptr;
    int *predecessors = (int *)predecessors_ptr;
    int *vertices_processed = (int *)vertices_processed_ptr;
    bool *verbose = (bool *)verbose_ptr;

    if (!g || !heap || !distances || !predecessors) {
        return event_result_failure("Missing required data", EC_ERROR_NULL_POINTER, ERROR_DETAIL_FULL);
    }

    if (verbose && *verbose) {
        printf("[EventChain] Event: Process vertices\n");
    }

    while (!heap_is_empty(heap)) {
        HeapNode current = heap_extract_min(heap);
        int u = current.vertex;
        (*vertices_processed)++;

        if (current.distance > distances[u]) {
            continue;
        }

        /* Relax edges */
        Edge *edge = g->adj_list[u];
        while (edge) {
            int v = edge->to;
            int alt = distances[u] + edge->weight;

            if (alt < distances[v]) {
                distances[v] = alt;
                predecessors[v] = u;

                if (heap->positions[v] == -1) {
                    heap_insert(heap, v, alt);
                } else {
                    heap_decrease_key(heap, v, alt);
                }
            }

            edge = edge->next;
        }
    }

    if (verbose && *verbose) {
        printf("[EventChain] Processed %d vertices\n", *vertices_processed);
    }

    return event_result_success();
}

/* Event 4: Cleanup */
EventResult event_cleanup(EventContext *context, void *user_data) {
    global_profile.context_lookups += 2;

    void *heap_ptr, *verbose_ptr;
    event_context_get(context, CTX_HEAP, &heap_ptr);
    event_context_get(context, CTX_VERBOSE, &verbose_ptr);

    MinHeap *heap = (MinHeap *)heap_ptr;
    bool *verbose = (bool *)verbose_ptr;

    if (verbose && *verbose) {
        printf("[EventChain] Event: Cleanup\n");
    }

    if (heap) {
        heap_destroy(heap);
    }

    return event_result_success();
}

/* Logging Middleware */
EventResult logging_middleware(
    ChainableEvent *event,
    EventContext *context,
    MiddlewareNextFunc next,
    void *next_data,
    void *user_data
) {
    global_profile.middleware_calls++;

    void *verbose_ptr;
    event_context_get(context, CTX_VERBOSE, &verbose_ptr);
    bool *verbose = (bool *)verbose_ptr;

    if (verbose && *verbose) {
        printf("[Middleware:Logging] Before: %s\n", event->name);
    }

    EventResult result = next(event, context, next_data);

    if (verbose && *verbose) {
        printf("[Middleware:Logging] After: %s (Success: %s)\n",
               event->name, result.success ? "YES" : "NO");
    }

    return result;
}

/* Timing Middleware */
typedef struct {
    uint64_t total_time;
    bool verbose;
} TimingData;

EventResult timing_middleware(
    ChainableEvent *event,
    EventContext *context,
    MiddlewareNextFunc next,
    void *next_data,
    void *user_data
) {
    global_profile.middleware_calls++;

    TimingData *timing = (TimingData *)user_data;

    uint64_t start = get_time_ns();
    EventResult result = next(event, context, next_data);
    uint64_t end = get_time_ns();

    uint64_t duration = end - start;
    timing->total_time += duration;

    if (timing->verbose) {
        printf("[Middleware:Timing] %s took %lu ns\n", event->name, (unsigned long)duration);
    }

    return result;
}

/* Performance Profiling Middleware */
EventResult profiling_middleware(
    ChainableEvent *event,
    EventContext *context,
    MiddlewareNextFunc next,
    void *next_data,
    void *user_data
) {
    global_profile.middleware_calls++;
    global_profile.event_wrapping_overhead += 50;  /* Estimated overhead in ns */

    size_t allocs_before = global_profile.num_allocations;
    size_t mem_before = global_profile.memory_allocated;

    EventResult result = next(event, context, next_data);

    size_t allocs_after = global_profile.num_allocations;
    size_t mem_after = global_profile.memory_allocated;

    void *verbose_ptr;
    event_context_get(context, CTX_VERBOSE, &verbose_ptr);
    bool *verbose = (bool *)verbose_ptr;

    if (verbose && *verbose) {
        printf("[Middleware:Profiling] %s: +%zu allocations, +%zu bytes\n",
               event->name,
               allocs_after - allocs_before,
               mem_after - mem_before);
    }

    return result;
}

DijkstraResult dijkstra_eventchains(Graph *g, int source, bool verbose, bool use_middleware) {
    DijkstraResult result;
    result.success = false;

    /* Create event chain */
    EventChain *chain = event_chain_create_strict();

    /* Add middleware if requested */
    TimingData timing_data = {0, verbose};

    if (use_middleware) {
        EventMiddleware *profiling = event_middleware_create(
            profiling_middleware, NULL, "ProfilingMiddleware");
        EventMiddleware *timing = event_middleware_create(
            timing_middleware, &timing_data, "TimingMiddleware");
        EventMiddleware *logging = event_middleware_create(
            logging_middleware, NULL, "LoggingMiddleware");

        event_chain_use_middleware(chain, profiling);
        event_chain_use_middleware(chain, timing);
        event_chain_use_middleware(chain, logging);
    }

    /* Create events */
    ChainableEvent *init = chainable_event_create(
        event_initialize, NULL, "InitializeDistances");
    ChainableEvent *create_heap_evt = chainable_event_create(
        event_create_heap, NULL, "CreatePriorityQueue");
    ChainableEvent *process = chainable_event_create(
        event_process_vertices, NULL, "ProcessVertices");
    ChainableEvent *cleanup = chainable_event_create(
        event_cleanup, NULL, "Cleanup");

    event_chain_add_event(chain, init);
    event_chain_add_event(chain, create_heap_evt);
    event_chain_add_event(chain, process);
    event_chain_add_event(chain, cleanup);

    /* Set up context */
    EventContext *ctx = event_chain_get_context(chain);
    event_context_set(ctx, CTX_GRAPH, g);
    event_context_set(ctx, CTX_SOURCE, &source);
    event_context_set(ctx, CTX_VERBOSE, &verbose);

    /* Execute chain */
    ChainResult chain_result = event_chain_execute(chain);

    if (chain_result.success) {
        void *distances_ptr, *predecessors_ptr;
        event_context_get(ctx, CTX_DISTANCES, &distances_ptr);
        event_context_get(ctx, CTX_PREDECESSORS, &predecessors_ptr);

        result.distances = (int *)distances_ptr;
        result.predecessors = (int *)predecessors_ptr;
        result.success = true;
    }

    if (verbose && use_middleware) {
        printf("[EventChain] Total middleware time: %lu ns\n", (unsigned long)timing_data.total_time);
    }

    chain_result_destroy(&chain_result);
    event_chain_destroy(chain);

    return result;
}

/* ==================== Benchmarking ==================== */

void run_benchmark(int num_vertices, int num_edges, unsigned int seed, bool verbose) {
    printf("\n========================================\n");
    printf("Benchmark: %d nodes, %d edges\n", num_vertices, num_edges);
    printf("========================================\n\n");

    /* Generate graph */
    Graph *g = graph_generate_random(num_vertices, num_edges, seed);
    int source = 0;

    /* ===== Traditional Implementation ===== */
    printf("--- Traditional Dijkstra ---\n");
    profile_reset(&global_profile);

    uint64_t trad_start = get_time_ns();
    DijkstraResult trad_result = dijkstra_traditional(g, source, verbose);
    uint64_t trad_end = get_time_ns();

    ProfileData trad_profile = global_profile;
    uint64_t trad_time = trad_end - trad_start;

    printf("Time: %.3f ms (%lu ns)\n", trad_time / 1000000.0, (unsigned long)trad_time);
    printf("Memory: %zu bytes (%zu allocations)\n",
           trad_profile.memory_allocated, trad_profile.num_allocations);

    /* ===== EventChains (Bare) ===== */
    printf("\n--- EventChains (Bare) ---\n");
    profile_reset(&global_profile);

    uint64_t bare_start = get_time_ns();
    DijkstraResult bare_result = dijkstra_eventchains(g, source, verbose, false);
    uint64_t bare_end = get_time_ns();

    ProfileData bare_profile = global_profile;
    uint64_t bare_time = bare_end - bare_start;

    printf("Time: %.3f ms (%lu ns)\n", bare_time / 1000000.0, (unsigned long)bare_time);
    printf("Memory: %zu bytes (%zu allocations)\n",
           bare_profile.memory_allocated, bare_profile.num_allocations);
    printf("Context lookups: %zu\n", bare_profile.context_lookups);

    /* ===== EventChains (Full Middleware) ===== */
    printf("\n--- EventChains (Full Middleware) ---\n");
    profile_reset(&global_profile);

    uint64_t full_start = get_time_ns();
    DijkstraResult full_result = dijkstra_eventchains(g, source, verbose, true);
    uint64_t full_end = get_time_ns();

    ProfileData full_profile = global_profile;
    uint64_t full_time = full_end - full_start;

    printf("Time: %.3f ms (%lu ns)\n", full_time / 1000000.0, (unsigned long)full_time);
    printf("Memory: %zu bytes (%zu allocations)\n",
           full_profile.memory_allocated, full_profile.num_allocations);
    printf("Context lookups: %zu\n", full_profile.context_lookups);
    printf("Middleware calls: %zu\n", full_profile.middleware_calls);
    printf("Event wrapping overhead: %zu ns (estimated)\n",
           full_profile.event_wrapping_overhead);

    /* ===== Overhead Analysis ===== */
    printf("\n--- Overhead Analysis ---\n");
    printf("Traditional (baseline): %.3f ms\n", trad_time / 1000000.0);

    int64_t bare_overhead = (int64_t)bare_time - (int64_t)trad_time;
    double bare_overhead_pct = ((double)bare_overhead / (double)trad_time) * 100.0;
    printf("EventChains (bare): %.3f ms (%+.1f%% overhead)\n",
           bare_time / 1000000.0, bare_overhead_pct);

    int64_t full_overhead = (int64_t)full_time - (int64_t)trad_time;
    double full_overhead_pct = ((double)full_overhead / (double)trad_time) * 100.0;
    printf("EventChains (full): %.3f ms (%+.1f%% overhead)\n",
           full_time / 1000000.0, full_overhead_pct);

    printf("\nOverhead breakdown:\n");

    printf("  Event wrapping: %+.1f%% (%+ld ns)\n",
           ((double)bare_overhead / (double)trad_time) * 100.0, (long)bare_overhead);

    uint64_t context_overhead = (bare_profile.context_lookups * 20);  /* ~20ns per lookup */
    printf("  Context lookups: +%.1f%% (%lu ns, %zu lookups)\n",
           ((double)context_overhead / (double)trad_time) * 100.0,
           (unsigned long)context_overhead, bare_profile.context_lookups);

    int64_t middleware_overhead = full_overhead - bare_overhead;
    printf("  Middleware calls: %+.1f%% (%+ld ns, %zu calls)\n",
           ((double)middleware_overhead / (double)trad_time) * 100.0,
           (long)middleware_overhead, full_profile.middleware_calls);
    
    /* Verify results match */
    printf("\n--- Verification ---\n");
    bool results_match = true;
    for (int i = 0; i < g->num_vertices && i < 10; i++) {
        if (trad_result.distances[i] != bare_result.distances[i] ||
            bare_result.distances[i] != full_result.distances[i]) {
            results_match = false;
            break;
        }
    }
    printf("Results match: %s\n", results_match ? "YES" : "NO");
    
    /* Cleanup */
    free(trad_result.distances);
    free(trad_result.predecessors);
    free(bare_result.distances);
    free(bare_result.predecessors);
    free(full_result.distances);
    free(full_result.predecessors);
    
    graph_destroy(g);
}

/* ==================== Main ==================== */

int main(int argc, char *argv[]) {
    bool verbose = false;
    
    if (argc > 1 && strcmp(argv[1], "-v") == 0) {
        verbose = true;
    }
    
    printf("===========================================\n");
    printf("Dijkstra's Algorithm: EventChains vs Traditional\n");
    printf("===========================================\n");
    
    /* Run benchmarks with different graph sizes */
    run_benchmark(100, 500, 42, verbose);
    run_benchmark(500, 2500, 42, verbose);
    run_benchmark(1000, 5000, 42, verbose);
    run_benchmark(2000, 10000, 42, verbose);
    
    return 0;
}