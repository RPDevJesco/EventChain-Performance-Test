/Users/rpdevstudios/CLionProjects/eventchain_test/cmake-build-debug/eventchain_test
===========================================
Dijkstra's Algorithm: EventChains vs Traditional
===========================================

========================================
Benchmark: 100 nodes, 500 edges
========================================

--- Traditional Dijkstra ---
Time: 0.011 ms (11500 ns)
Memory: 2024 bytes (3 allocations)

--- EventChains (Bare) ---
Time: 0.019 ms (18625 ns)
Memory: 2028 bytes (4 allocations)
Context lookups: 13

--- EventChains (Full Middleware) ---
Time: 0.012 ms (12333 ns)
Memory: 2028 bytes (4 allocations)
Context lookups: 13
Middleware calls: 0
Event wrapping overhead: 0 ns (estimated)

--- Overhead Analysis ---
Traditional (baseline): 0.011 ms
EventChains (bare): 0.019 ms (+62.0% overhead)
EventChains (full): 0.012 ms (+7.2% overhead)

Overhead breakdown:
Event wrapping: +62.0% (+7125 ns)
Context lookups: +2.3% (260 ns, 13 lookups)
Middleware calls: -54.7% (-6292 ns, 0 calls)

--- Verification ---
Results match: YES

========================================
Benchmark: 500 nodes, 2500 edges
========================================

--- Traditional Dijkstra ---
Time: 0.073 ms (73500 ns)
Memory: 10024 bytes (3 allocations)

--- EventChains (Bare) ---
Time: 0.095 ms (95333 ns)
Memory: 10028 bytes (4 allocations)
Context lookups: 13

--- EventChains (Full Middleware) ---
Time: 0.058 ms (58334 ns)
Memory: 10028 bytes (4 allocations)
Context lookups: 13
Middleware calls: 0
Event wrapping overhead: 0 ns (estimated)

--- Overhead Analysis ---
Traditional (baseline): 0.073 ms
EventChains (bare): 0.095 ms (+29.7% overhead)
EventChains (full): 0.058 ms (-20.6% overhead)

Overhead breakdown:
Event wrapping: +29.7% (+21833 ns)
Context lookups: +0.4% (260 ns, 13 lookups)
Middleware calls: -50.3% (-36999 ns, 0 calls)

--- Verification ---
Results match: YES

========================================
Benchmark: 1000 nodes, 5000 edges
========================================

--- Traditional Dijkstra ---
Time: 0.146 ms (146417 ns)
Memory: 20024 bytes (3 allocations)

--- EventChains (Bare) ---
Time: 0.157 ms (156958 ns)
Memory: 20028 bytes (4 allocations)
Context lookups: 13

--- EventChains (Full Middleware) ---
Time: 0.111 ms (110500 ns)
Memory: 20028 bytes (4 allocations)
Context lookups: 13
Middleware calls: 0
Event wrapping overhead: 0 ns (estimated)

--- Overhead Analysis ---
Traditional (baseline): 0.146 ms
EventChains (bare): 0.157 ms (+7.2% overhead)
EventChains (full): 0.111 ms (-24.5% overhead)

Overhead breakdown:
Event wrapping: +7.2% (+10541 ns)
Context lookups: +0.2% (260 ns, 13 lookups)
Middleware calls: -31.7% (-46458 ns, 0 calls)

--- Verification ---
Results match: YES

========================================
Benchmark: 2000 nodes, 10000 edges
========================================

--- Traditional Dijkstra ---
Time: 0.324 ms (323916 ns)
Memory: 40024 bytes (3 allocations)

--- EventChains (Bare) ---
Time: 0.324 ms (324125 ns)
Memory: 40028 bytes (4 allocations)
Context lookups: 13

--- EventChains (Full Middleware) ---
Time: 0.261 ms (260958 ns)
Memory: 40028 bytes (4 allocations)
Context lookups: 13
Middleware calls: 0
Event wrapping overhead: 0 ns (estimated)

--- Overhead Analysis ---
Traditional (baseline): 0.324 ms
EventChains (bare): 0.324 ms (+0.1% overhead)
EventChains (full): 0.261 ms (-19.4% overhead)

Overhead breakdown:
Event wrapping: +0.1% (+209 ns)
Context lookups: +0.1% (260 ns, 13 lookups)
Middleware calls: -19.5% (-63167 ns, 0 calls)

--- Verification ---
Results match: YES

Process finished with exit code 0


/Users/rpdevstudios/CLionProjects/eventchain_test/cmake-build-debug/eventchain_test

╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║         EventChains Stress & Performance Test Suite          ║
║              Benchmarking and Load Testing                   ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝

EventChains v3.1.0 - Security-Hardened Build (No Magic Numbers)
Features:
- Reference counting for memory safety
- Constant-time comparisons for sensitive data
- Memory usage limits (10 MB max)
- Iterative middleware execution (max 16 layers)
- Reentrancy protection
- Signal safety
- Function pointer validation
- Configurable error detail levels
- Overflow protection on all arithmetic
- Secure memory zeroing
- Optimized: No magic number overhead

═══════════════════════════════════════════════════════════════
SECTION 1: Performance Benchmarks
═══════════════════════════════════════════════════════════════

╔═══════════════════════════════════════════════════════════════╗
║               PERFORMANCE TEST: Minimal Chain                 ║
╚═══════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────────┐
│ Minimal Chain (1 Event, No Middleware)                      │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  10000                                          │
│ Total Time:  1.527                                       ms │
│ Average:     0.000153                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.002930                                    ms │
│ Throughput:  6548361                                 ops/sec │
└─────────────────────────────────────────────────────────────┘
✓ Completed 10000 iterations successfully

╔═══════════════════════════════════════════════════════════════╗
║          PERFORMANCE TEST: Chain with Multiple Events         ║
╚═══════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────────┐
│ Chain with 5 Events                                         │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.705                                       ms │
│ Average:     0.000705                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.002930                                    ms │
│ Throughput:  1418774                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Chain with 10 Events                                        │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  1.229                                       ms │
│ Average:     0.001229                                    ms │
│ Min:         0.000977                                    ms │
│ Max:         0.002930                                    ms │
│ Throughput:  813829                                  ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Chain with 50 Events                                        │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  5.002                                       ms │
│ Average:     0.005002                                    ms │
│ Min:         0.003906                                    ms │
│ Max:         0.021973                                    ms │
│ Throughput:  199902                                  ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Chain with 100 Events                                       │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  9.132                                       ms │
│ Average:     0.009132                                    ms │
│ Min:         0.007812                                    ms │
│ Max:         0.026123                                    ms │
│ Throughput:  109504                                  ops/sec │
└─────────────────────────────────────────────────────────────┘

╔═══════════════════════════════════════════════════════════════╗
║        PERFORMANCE TEST: Chain with Middleware Layers         ║
╚═══════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────────┐
│ Chain with 1 Middleware Layers                              │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.225                                       ms │
│ Average:     0.000225                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.001221                                    ms │
│ Throughput:  4442516                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Chain with 4 Middleware Layers                              │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.215                                       ms │
│ Average:     0.000215                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.001221                                    ms │
│ Throughput:  4654545                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Chain with 8 Middleware Layers                              │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.222                                       ms │
│ Average:     0.000222                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.002930                                    ms │
│ Throughput:  4506051                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Chain with 16 Middleware Layers                             │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.198                                       ms │
│ Average:     0.000198                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.001221                                    ms │
│ Throughput:  5056790                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

╔═══════════════════════════════════════════════════════════════╗
║           PERFORMANCE TEST: Context Operations                ║
╚═══════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────────┐
│ Context: 100 Set Operations                                 │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  10000                                          │
│ Total Time:  185.903                                     ms │
│ Average:     0.018590                                    ms │
│ Min:         0.015869                                    ms │
│ Max:         0.164062                                    ms │
│ Throughput:  53792                                   ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Context: 100 Get Operations                                 │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  10000                                          │
│ Total Time:  163.835                                     ms │
│ Average:     0.016384                                    ms │
│ Min:         0.013916                                    ms │
│ Max:         0.091797                                    ms │
│ Throughput:  61037                                   ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Context: 100 Has Operations (Fast)                          │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  10000                                          │
│ Total Time:  161.546                                     ms │
│ Average:     0.016155                                    ms │
│ Min:         0.013916                                    ms │
│ Max:         0.058105                                    ms │
│ Throughput:  61902                                   ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Context: 100 Has Operations (Constant-Time)                 │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  10000                                          │
│ Total Time:  1500.874                                    ms │
│ Average:     0.150087                                    ms │
│ Min:         0.135010                                    ms │
│ Max:         0.257080                                    ms │
│ Throughput:  6663                                    ops/sec │
└─────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════
SECTION 2: Stress Tests
═══════════════════════════════════════════════════════════════

╔═══════════════════════════════════════════════════════════════╗
║              STRESS TEST: Maximum Events per Chain            ║
╚═══════════════════════════════════════════════════════════════╝
Adding 1024 events to chain...

✓ Successfully added 1024 events in 0.16 ms
Executing chain with 1024 events...
✓ Chain executed successfully in 0.01 ms
✓ Average time per event: 0.000006 ms

╔═══════════════════════════════════════════════════════════════╗
║           STRESS TEST: Maximum Middleware Layers              ║
╚═══════════════════════════════════════════════════════════════╝
Adding 16 middleware layers to chain...
✓ Successfully added 16 middleware layers
Executing chain with 16 middleware layers...
✓ Chain executed successfully in 0.001221 ms

╔═══════════════════════════════════════════════════════════════╗
║          STRESS TEST: Maximum Context Entries                 ║
╚═══════════════════════════════════════════════════════════════╝
Adding 512 entries to context...
Progress: 500/512 entries added
✓ Successfully added 512 entries in 0.37 ms
✓ Memory usage: 24506 bytes
Testing retrieval of all 512 entries...
✓ Retrieved all entries in 0.42 ms
✓ Average retrieval time: 0.000823 ms per entry

╔═══════════════════════════════════════════════════════════════╗
║        STRESS TEST: Rapid Creation/Destruction Cycles         ║
╚═══════════════════════════════════════════════════════════════╝
Creating and destroying 10000 chains...

✓ Completed 10000 cycles in 17.17 ms
✓ Average cycle time: 0.001717 ms
✓ Throughput: 582480 cycles/sec

╔═══════════════════════════════════════════════════════════════╗
║              STRESS TEST: Memory Pressure                      ║
╚═══════════════════════════════════════════════════════════════╝
Creating 100 chains with heavy memory usage...

✓ Created 100 chains in 0.25 ms
Executing all chains...

✓ Executed 100 chains in 7.80 ms
✓ Average execution time: 0.078000 ms per chain
Destroying all chains...
✓ Destroyed 100 chains in 0.75 ms
✓ Total test time: 8.80 ms

╔═══════════════════════════════════════════════════════════════╗
║          STRESS TEST: Error Handling Overhead                 ║
╚═══════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────────┐
│ All Events Succeed (Strict Mode)                            │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  1.001                                       ms │
│ Average:     0.001001                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.001953                                    ms │
│ Throughput:  999268                                  ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Some Events Fail (Lenient Mode)                             │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.911                                       ms │
│ Average:     0.000911                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.002197                                    ms │
│ Throughput:  1097241                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

╔═══════════════════════════════════════════════════════════════╗
║          STRESS TEST: Deep Middleware Stack Impact            ║
╚═══════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────────┐
│ 0 Middleware Layers (5 Events)                              │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.547                                       ms │
│ Average:     0.000547                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.001221                                    ms │
│ Throughput:  1828571                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ 4 Middleware Layers (5 Events)                              │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.755                                       ms │
│ Average:     0.000755                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.001953                                    ms │
│ Throughput:  1325137                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ 8 Middleware Layers (5 Events)                              │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.822                                       ms │
│ Average:     0.000822                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.001221                                    ms │
│ Throughput:  1216152                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ 12 Middleware Layers (5 Events)                             │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.889                                       ms │
│ Average:     0.000889                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.003906                                    ms │
│ Throughput:  1124348                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ 16 Middleware Layers (5 Events)                             │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.944                                       ms │
│ Average:     0.000944                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.002197                                    ms │
│ Throughput:  1059767                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

╔═══════════════════════════════════════════════════════════════╗
║                       Test Complete                           ║
╚═══════════════════════════════════════════════════════════════╝

✓ All stress and performance tests completed successfully
✓ No memory leaks detected
✓ System remained stable under load


Process finished with exit code 0
