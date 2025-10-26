PS C:\Users\jglov\CLionProjects\eventchain_test\Debug> ./multi_tier_benchmark.exe

|---------------------------------------------------------------|
|                                                               |
|        EventChains Multi-Tier Benchmark Suite                |
|         Measuring Abstraction Overhead at Scale              |
|                                                               |
|---------------------------------------------------------------|

EventChains v3.1.0 - Security-Hardened Build (No Magic Numbers)
Features:
- Reference counting for memory safety
- Constant-time comparisons for sensitive data
- Memory usage limits (14757395255542153216 bytes, ~10 MB max)
- Iterative middleware execution (max 16 layers)
- Reentrancy protection
- Signal safety
- Function pointer validation
- Configurable error detail levels
- Overflow protection on all arithmetic
- Secure memory zeroing
- Optimized: No magic number overhead

Benchmark Configuration:
Iterations per test: 10000
Timing precision: nanoseconds
Work per event: ~100 operations + string formatting

|---------------------------------------------------------------|
|  TIER 1: Minimal Baseline (Cost of Orchestration Framework)  |
|---------------------------------------------------------------|

Baseline: 3 bare function calls
EventChains: Full pattern with 0 middleware
Iterations: 10000

Results:
----------------------------------------------------------------
Baseline (3 function calls)        : avg=  1.233 us  min=  1.100 us  max=  6.100 us  stddev=-0.256 us
EventChains (0 middleware)         : avg=  2.012 us  min=  1.900 us  max= 28.000 us  stddev=-0.256 us

EventChains Overhead               :  +63.18% ( +0.779 us overhead)

|---------------------------------------------------------------|
|   TIER 2: Feature-Parity (Cost of Abstraction vs Manual)     |
|---------------------------------------------------------------|

Baseline: Manual error handling + name tracking + cleanup
EventChains: Full pattern with 0 middleware
Iterations: 10000

Results:
----------------------------------------------------------------
Baseline (manual equivalent)       : avg=  1.403 us  min=  1.300 us  max=  7.400 us  stddev=-0.256 us
EventChains (0 middleware)         : avg=  1.955 us  min=  1.800 us  max= 10.100 us  stddev=-0.256 us

EventChains Overhead               :  +39.34% ( +0.552 us overhead)

|---------------------------------------------------------------|
|      TIER 3: Middleware Scaling (Cost per Middleware)        |
|---------------------------------------------------------------|

EventChains: 0, 1, 3, 5, 10 middleware layers
Iterations: 10000 per configuration

Results:
----------------------------------------------------------------
0 middleware layers                : avg=  1.920 us  min=  1.800 us  max=  6.100 us  stddev=-0.256 us
1 middleware layer                 : avg=  2.488 us  min=  2.300 us  max= 40.400 us  stddev=-0.256 us
3 middleware layers                : avg=  2.516 us  min=  2.300 us  max= 24.500 us  stddev=-0.256 us
5 middleware layers                : avg=  2.562 us  min=  2.400 us  max= 16.900 us  stddev=-0.256 us
10 middleware layers               : avg=  2.694 us  min=  2.500 us  max= 38.600 us  stddev=-0.256 us

Overhead Analysis:
----------------------------------------------------------------
0 -> 1 middleware:  +0.568 us total, 0.568 us per layer
1 -> 3 middleware:  +0.028 us total, 0.014 us per layer
3 -> 5 middleware:  +0.046 us total, 0.023 us per layer
5 -> 10 middleware:  +0.132 us total, 0.026 us per layer

Amortized cost per middleware layer: 0.077 us

|---------------------------------------------------------------|
|  TIER 4: Real-World (Cost vs Manual Instrumentation)         |
|---------------------------------------------------------------|

Baseline: Manual logging + timing implementation
EventChains: Logging + timing middleware
Iterations: 10000

Results:
----------------------------------------------------------------
Baseline (manual instrumentation)  : avg=  1.320 us  min=  1.200 us  max= 10.400 us  stddev=-0.256 us
EventChains (middleware-based)     : avg=  2.600 us  min=  2.400 us  max=468.100 us  stddev=-0.256 us

EventChains Overhead               :  +96.97% ( +1.280 us overhead)

|---------------------------------------------------------------|
|                     Benchmark Complete                       |
|---------------------------------------------------------------|

Key Findings:
Tier 1 shows raw orchestration framework overhead
Tier 2 shows abstraction cost vs feature-equivalent manual code
Tier 3 quantifies cost per middleware layer (amortized)
Tier 4 demonstrates real-world instrumentation scenarios

PS C:\Users\jglov\CLionProjects\eventchain_test\Debug> 