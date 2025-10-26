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
- Memory usage limits (2259152797706 MB max)
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
Baseline (3 function calls)        : avg=  0.720 us  min=  0.600 us  max= 13.000 us  stddev= 0.168 us
EventChains (0 middleware)         : avg=  0.941 us  min=  0.800 us  max= 26.800 us  stddev= 0.295 us

EventChains Overhead               :  +30.69% ( +0.221 us overhead)

|---------------------------------------------------------------|
|   TIER 2: Feature-Parity (Cost of Abstraction vs Manual)     |
|---------------------------------------------------------------|

Baseline: Manual error handling + name tracking + cleanup
EventChains: Full pattern with 0 middleware
Iterations: 10000

Results:
----------------------------------------------------------------
Baseline (manual equivalent)       : avg=  0.824 us  min=  0.700 us  max= 11.600 us  stddev= 0.132 us
EventChains (0 middleware)         : avg=  0.942 us  min=  0.800 us  max=  6.300 us  stddev= 0.109 us

EventChains Overhead               :  +14.32% ( +0.118 us overhead)

|---------------------------------------------------------------|
|      TIER 3: Middleware Scaling (Cost per Middleware)        |
|---------------------------------------------------------------|

EventChains: 0, 1, 3, 5, 10 middleware layers
Iterations: 10000 per configuration

Results:
----------------------------------------------------------------
0 middleware layers                : avg=  0.926 us  min=  0.800 us  max= 62.100 us  stddev= 0.617 us
1 middleware layer                 : avg=  1.116 us  min=  1.000 us  max=  6.700 us  stddev= 0.122 us
3 middleware layers                : avg=  1.165 us  min=  1.000 us  max=  6.200 us  stddev= 0.145 us
5 middleware layers                : avg=  1.160 us  min=  1.000 us  max= 47.300 us  stddev= 0.483 us
10 middleware layers               : avg=  1.220 us  min=  1.100 us  max=  8.500 us  stddev= 0.149 us

Overhead Analysis:
----------------------------------------------------------------
0 -> 1 middleware:  +0.190 us total, 0.190 us per layer
1 -> 3 middleware:  +0.049 us total, 0.025 us per layer
3 -> 5 middleware:  -0.005 us total, -0.003 us per layer
5 -> 10 middleware:  +0.060 us total, 0.012 us per layer

Amortized cost per middleware layer: 0.029 us

|---------------------------------------------------------------|
|  TIER 4: Real-World (Cost vs Manual Instrumentation)         |
|---------------------------------------------------------------|

Baseline: Manual logging + timing implementation
EventChains: Logging + timing middleware
Iterations: 10000

Results:
----------------------------------------------------------------
Baseline (manual instrumentation)  : avg=  0.809 us  min=  0.700 us  max=  5.900 us  stddev= 0.083 us
EventChains (middleware-based)     : avg=  1.137 us  min=  1.000 us  max= 62.300 us  stddev= 0.629 us

EventChains Overhead               :  +40.54% ( +0.328 us overhead)

|---------------------------------------------------------------|
|                     Benchmark Complete                       |
|---------------------------------------------------------------|

Key Findings:
Tier 1 shows raw orchestration framework overhead
Tier 2 shows abstraction cost vs feature-equivalent manual code
Tier 3 quantifies cost per middleware layer (amortized)
Tier 4 demonstrates real-world instrumentation scenarios