### M2 MacBook Pro with 16GB of RAM running macOS Tahoe Version 26.0.1

/Users/gdmejesco/CLionProjects/eventchain_test/cmake-build-debug/eventchain_test
===========================================
Dijkstra's Algorithm: EventChains vs Traditional
===========================================
========================================
Benchmark: 100 nodes, 500 edges
========================================
--- Traditional Dijkstra ---
Time: 0.014 ms (13958 ns)
Memory: 2024 bytes (3 allocations)
--- EventChains (Bare) ---
Time: 0.023 ms (23375 ns)
Memory: 2028 bytes (4 allocations)
Context lookups: 13
--- EventChains (Full Middleware) ---
Time: 0.019 ms (19458 ns)
Memory: 2028 bytes (4 allocations)
Context lookups: 13
Middleware calls: 0
Event wrapping overhead: 0 ns (estimated)
--- Overhead Analysis ---
Traditional (baseline): 0.014 ms
EventChains (bare): 0.023 ms (+67.5% overhead)
EventChains (full): 0.019 ms (+39.4% overhead)
Overhead breakdown:
Event wrapping: +67.5% (+9417 ns)
Context lookups: +1.9% (260 ns, 13 lookups)
Middleware calls: -28.1% (-3917 ns, 0 calls)
--- Verification ---
Results match: YES
========================================
Benchmark: 500 nodes, 2500 edges
========================================
--- Traditional Dijkstra ---
Time: 0.139 ms (139459 ns)
Memory: 10024 bytes (3 allocations)
--- EventChains (Bare) ---
Time: 0.085 ms (84583 ns)
Memory: 10028 bytes (4 allocations)
Context lookups: 13
--- EventChains (Full Middleware) ---
Time: 0.089 ms (89500 ns)
Memory: 10028 bytes (4 allocations)
Context lookups: 13
Middleware calls: 0
Event wrapping overhead: 0 ns (estimated)
--- Overhead Analysis ---
Traditional (baseline): 0.139 ms
EventChains (bare): 0.085 ms (-39.3% overhead)
EventChains (full): 0.089 ms (-35.8% overhead)
Overhead breakdown:
Event wrapping: -39.3% (-54876 ns)
Context lookups: +0.2% (260 ns, 13 lookups)
Middleware calls: +3.5% (+4917 ns, 0 calls)
--- Verification ---
Results match: YES
========================================
Benchmark: 1000 nodes, 5000 edges
========================================
--- Traditional Dijkstra ---
Time: 0.171 ms (170583 ns)
Memory: 20024 bytes (3 allocations)
--- EventChains (Bare) ---
Time: 0.181 ms (181417 ns)
Memory: 20028 bytes (4 allocations)
Context lookups: 13
--- EventChains (Full Middleware) ---
Time: 0.276 ms (276208 ns)
Memory: 20028 bytes (4 allocations)
Context lookups: 13
Middleware calls: 0
Event wrapping overhead: 0 ns (estimated)
--- Overhead Analysis ---
Traditional (baseline): 0.171 ms
EventChains (bare): 0.181 ms (+6.4% overhead)
EventChains (full): 0.276 ms (+61.9% overhead)
Overhead breakdown:
Event wrapping: +6.4% (+10834 ns)
Context lookups: +0.2% (260 ns, 13 lookups)
Middleware calls: +55.6% (+94791 ns, 0 calls)
--- Verification ---
Results match: YES
========================================
Benchmark: 2000 nodes, 10000 edges
========================================
--- Traditional Dijkstra ---
Time: 0.374 ms (373875 ns)
Memory: 40024 bytes (3 allocations)
--- EventChains (Bare) ---
Time: 0.377 ms (376875 ns)
Memory: 40028 bytes (4 allocations)
Context lookups: 13
--- EventChains (Full Middleware) ---
Time: 0.372 ms (371667 ns)
Memory: 40028 bytes (4 allocations)
Context lookups: 13
Middleware calls: 0
Event wrapping overhead: 0 ns (estimated)
--- Overhead Analysis ---
Traditional (baseline): 0.374 ms
EventChains (bare): 0.377 ms (+0.8% overhead)
EventChains (full): 0.372 ms (-0.6% overhead)
Overhead breakdown:
Event wrapping: +0.8% (+3000 ns)
Context lookups: +0.1% (260 ns, 13 lookups)
Middleware calls: -1.4% (-5208 ns, 0 calls)
--- Verification ---
Results match: YES
Process finished with exit code 0

/Users/gdmejesco/CLionProjects/eventchain_test/cmake-build-debug/eventchain_test

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
│ Total Time:  2.687                                       ms │
│ Average:     0.000269                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.008057                                    ms │
│ Throughput:  3721606                                 ops/sec │
└─────────────────────────────────────────────────────────────┘
✓ Completed 10000 iterations successfully

╔═══════════════════════════════════════════════════════════════╗
║          PERFORMANCE TEST: Chain with Multiple Events         ║
╚═══════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────────┐
│ Chain with 5 Events                                         │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  1.495                                       ms │
│ Average:     0.001495                                    ms │
│ Min:         0.000977                                    ms │
│ Max:         0.003906                                    ms │
│ Throughput:  668953                                  ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Chain with 10 Events                                        │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  2.676                                       ms │
│ Average:     0.002676                                    ms │
│ Min:         0.001953                                    ms │
│ Max:         0.048828                                    ms │
│ Throughput:  373654                                  ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Chain with 50 Events                                        │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  9.047                                       ms │
│ Average:     0.009047                                    ms │
│ Min:         0.007812                                    ms │
│ Max:         0.023926                                    ms │
│ Throughput:  110529                                  ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Chain with 100 Events                                       │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  16.138                                      ms │
│ Average:     0.016138                                    ms │
│ Min:         0.014893                                    ms │
│ Max:         0.076904                                    ms │
│ Throughput:  61967                                   ops/sec │
└─────────────────────────────────────────────────────────────┘

╔═══════════════════════════════════════════════════════════════╗
║        PERFORMANCE TEST: Chain with Middleware Layers         ║
╚═══════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────────┐
│ Chain with 1 Middleware Layers                              │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.262                                       ms │
│ Average:     0.000262                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.011963                                    ms │
│ Throughput:  3820896                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Chain with 4 Middleware Layers                              │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.253                                       ms │
│ Average:     0.000253                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.007080                                    ms │
│ Throughput:  3949855                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Chain with 8 Middleware Layers                              │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.248                                       ms │
│ Average:     0.000248                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.001953                                    ms │
│ Throughput:  4027532                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Chain with 16 Middleware Layers                             │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.263                                       ms │
│ Average:     0.000263                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.005127                                    ms │
│ Throughput:  3806691                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

╔═══════════════════════════════════════════════════════════════╗
║           PERFORMANCE TEST: Context Operations                ║
╚═══════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────────┐
│ Context: 100 Set Operations                                 │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  10000                                          │
│ Total Time:  262.852                                     ms │
│ Average:     0.026285                                    ms │
│ Min:         0.023926                                    ms │
│ Max:         0.179932                                    ms │
│ Throughput:  38044                                   ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Context: 100 Get Operations                                 │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  10000                                          │
│ Total Time:  227.796                                     ms │
│ Average:     0.022780                                    ms │
│ Min:         0.021973                                    ms │
│ Max:         0.066895                                    ms │
│ Throughput:  43899                                   ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Context: 100 Has Operations (Fast)                          │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  10000                                          │
│ Total Time:  227.339                                     ms │
│ Average:     0.022734                                    ms │
│ Min:         0.020996                                    ms │
│ Max:         0.072021                                    ms │
│ Throughput:  43987                                   ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Context: 100 Has Operations (Constant-Time)                 │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  10000                                          │
│ Total Time:  1915.881                                    ms │
│ Average:     0.191588                                    ms │
│ Min:         0.185059                                    ms │
│ Max:         0.645020                                    ms │
│ Throughput:  5220                                    ops/sec │
└─────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════
SECTION 2: Stress Tests
═══════════════════════════════════════════════════════════════

╔═══════════════════════════════════════════════════════════════╗
║              STRESS TEST: Maximum Events per Chain            ║
╚═══════════════════════════════════════════════════════════════╝
Adding 1024 events to chain...

✓ Successfully added 1024 events in 0.23 ms
Executing chain with 1024 events...
✓ Chain executed successfully in 0.01 ms
✓ Average time per event: 0.000014 ms

╔═══════════════════════════════════════════════════════════════╗
║           STRESS TEST: Maximum Middleware Layers              ║
╚═══════════════════════════════════════════════════════════════╝
Adding 16 middleware layers to chain...
✓ Successfully added 16 middleware layers
Executing chain with 16 middleware layers...
✓ Chain executed successfully in 0.000977 ms

╔═══════════════════════════════════════════════════════════════╗
║          STRESS TEST: Maximum Context Entries                 ║
╚═══════════════════════════════════════════════════════════════╝
Adding 512 entries to context...
Progress: 500/512 entries added
✓ Successfully added 512 entries in 0.57 ms
✓ Memory usage: 24506 bytes
Testing retrieval of all 512 entries...
✓ Retrieved all entries in 0.47 ms
✓ Average retrieval time: 0.000918 ms per entry

╔═══════════════════════════════════════════════════════════════╗
║        STRESS TEST: Rapid Creation/Destruction Cycles         ║
╚═══════════════════════════════════════════════════════════════╝
Creating and destroying 10000 chains...

✓ Completed 10000 cycles in 63.69 ms
✓ Average cycle time: 0.006369 ms
✓ Throughput: 157006 cycles/sec

╔═══════════════════════════════════════════════════════════════╗
║              STRESS TEST: Memory Pressure                      ║
╚═══════════════════════════════════════════════════════════════╝
Creating 100 chains with heavy memory usage...

✓ Created 100 chains in 0.38 ms
Executing all chains...

✓ Executed 100 chains in 12.09 ms
✓ Average execution time: 0.120940 ms per chain
Destroying all chains...
✓ Destroyed 100 chains in 3.39 ms
✓ Total test time: 15.87 ms

╔═══════════════════════════════════════════════════════════════╗
║          STRESS TEST: Error Handling Overhead                 ║
╚═══════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────────┐
│ All Events Succeed (Strict Mode)                            │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  1.757                                       ms │
│ Average:     0.001757                                    ms │
│ Min:         0.000977                                    ms │
│ Max:         0.003906                                    ms │
│ Throughput:  569047                                  ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ Some Events Fail (Lenient Mode)                             │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  1.489                                       ms │
│ Average:     0.001489                                    ms │
│ Min:         0.000977                                    ms │
│ Max:         0.023193                                    ms │
│ Throughput:  671475                                  ops/sec │
└─────────────────────────────────────────────────────────────┘

╔═══════════════════════════════════════════════════════════════╗
║          STRESS TEST: Deep Middleware Stack Impact            ║
╚═══════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────────┐
│ 0 Middleware Layers (5 Events)                              │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  0.981                                       ms │
│ Average:     0.000981                                    ms │
│ Min:         0.000000                                    ms │
│ Max:         0.015869                                    ms │
│ Throughput:  1019666                                 ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ 4 Middleware Layers (5 Events)                              │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  1.133                                       ms │
│ Average:     0.001133                                    ms │
│ Min:         0.000977                                    ms │
│ Max:         0.002197                                    ms │
│ Throughput:  882759                                  ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ 8 Middleware Layers (5 Events)                              │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  1.237                                       ms │
│ Average:     0.001237                                    ms │
│ Min:         0.000977                                    ms │
│ Max:         0.002197                                    ms │
│ Throughput:  808208                                  ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ 12 Middleware Layers (5 Events)                             │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  1.596                                       ms │
│ Average:     0.001596                                    ms │
│ Min:         0.000977                                    ms │
│ Max:         0.008057                                    ms │
│ Throughput:  626683                                  ops/sec │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ 16 Middleware Layers (5 Events)                             │
├─────────────────────────────────────────────────────────────┤
│ Iterations:  1000                                           │
│ Total Time:  1.478                                       ms │
│ Average:     0.001478                                    ms │
│ Min:         0.000977                                    ms │
│ Max:         0.007080                                    ms │
│ Throughput:  676466                                  ops/sec │
└─────────────────────────────────────────────────────────────┘

╔═══════════════════════════════════════════════════════════════╗
║                       Test Complete                           ║
╚═══════════════════════════════════════════════════════════════╝

✓ All stress and performance tests completed successfully
✓ No memory leaks detected
✓ System remained stable under load


Process finished with exit code 0