# Windows Test Results

## System Specifications
- **CPU:** AMD Ryzen 9 7950X
- **RAM:** 128GB
- **OS:** Windows 11 Pro (Build 26100.6899)
- **Compiler Note:** Required custom `strndup()` implementation (not available on Windows)

---

## Dijkstra's Algorithm Benchmark

### 100 nodes, 500 edges
| Implementation | Time (ms) | Memory | Overhead |
|----------------|-----------|--------|----------|
| Traditional | 0.018 | 2024 bytes | baseline |
| EventChains (bare) | 0.051 | 2028 bytes | +178.7% |
| EventChains (full) | 0.036 | 2028 bytes | +94.5% |

### 500 nodes, 2500 edges
| Implementation | Time (ms) | Memory | Overhead |
|----------------|-----------|--------|----------|
| Traditional | 0.158 | 10024 bytes | baseline |
| EventChains (bare) | 0.161 | 10028 bytes | +2.0% |
| EventChains (full) | 0.166 | 10028 bytes | +5.1% |

### 1000 nodes, 5000 edges
| Implementation | Time (ms) | Memory | Overhead |
|----------------|-----------|--------|----------|
| Traditional | 0.273 | 20024 bytes | baseline |
| EventChains (bare) | 0.199 | 20028 bytes | **-27.4%** |
| EventChains (full) | 0.281 | 20028 bytes | +2.7% |

### 2000 nodes, 10000 edges
| Implementation | Time (ms) | Memory | Overhead |
|----------------|-----------|--------|----------|
| Traditional | 0.857 | 40024 bytes | baseline |
| EventChains (bare) | 0.685 | 40028 bytes | **-20.1%** |
| EventChains (full) | 0.651 | 40028 bytes | **-24.1%** |

**Key Finding:** EventChains shows negative overhead (faster than traditional) on larger graphs!

---

## Performance Benchmarks

### Minimal Chain (1 Event, No Middleware)
- **Iterations:** 10,000
- **Average:** 0.000155 ms
- **Throughput:** 6,438,227 ops/sec

### Chain with Multiple Events
| Events | Avg Time (ms) | Throughput (ops/sec) |
|--------|---------------|----------------------|
| 5 | 0.000845 | 1,183,815 |
| 10 | 0.001491 | 670,596 |
| 50 | 0.007063 | 141,588 |
| 100 | 0.014216 | 70,345 |

### Chain with Middleware Layers
| Middleware | Avg Time (ms) | Throughput (ops/sec) |
|------------|---------------|----------------------|
| 1 | 0.000214 | 4,681,143 |
| 4 | 0.000241 | 4,141,557 |
| 8 | 0.000309 | 3,237,945 |
| 16 | 0.000274 | 3,653,880 |

### Context Operations (10,000 iterations)
| Operation | Avg Time (ms) | Throughput (ops/sec) |
|-----------|---------------|----------------------|
| 100 Set | 0.029621 | 33,760 |
| 100 Get | 0.026613 | 37,575 |
| 100 Has (Fast) | 0.025233 | 39,631 |
| 100 Has (Constant-Time) | 0.145877 | 6,855 |

---

## Stress Tests

### Maximum Events per Chain
- **Capacity:** 1,024 events
- **Add Time:** 10.97 ms
- **Execute Time:** 0.04 ms
- **Avg per Event:** 0.000035 ms

### Maximum Middleware Layers
- **Capacity:** 16 layers
- **Execute Time:** 0.002930 ms

### Maximum Context Entries
- **Capacity:** 512 entries
- **Add Time:** 11.33 ms
- **Memory Usage:** 24,506 bytes
- **Retrieval Time:** 0.50 ms (0.000978 ms per entry)

### Rapid Creation/Destruction
- **Cycles:** 10,000
- **Total Time:** 37.85 ms
- **Throughput:** 264,209 cycles/sec

### Memory Pressure Test
- **Chains:** 100 (50 events + 8 middleware each)
- **Create Time:** 12.08 ms
- **Execute Time:** 22.03 ms (0.220 ms per chain)
- **Destroy Time:** 1.58 ms
- **Total:** 35.70 ms

### Error Handling Overhead (1,000 iterations)
| Test | Avg Time (ms) | Throughput (ops/sec) |
|------|---------------|----------------------|
| All Succeed (Strict) | 0.001482 | 674,794 |
| Some Fail (Lenient) | 0.001255 | 796,577 |

### Deep Middleware Stack (5 Events, 1,000 iterations)
| Middleware Layers | Avg Time (ms) | Throughput (ops/sec) |
|-------------------|---------------|----------------------|
| 0 | 0.000781 | 1,280,801 |
| 4 | 0.001192 | 839,000 |
| 8 | 0.001337 | 747,718 |
| 12 | 0.001449 | 690,027 |
| 16 | 0.001391 | 718,849 |

---

## Summary
✓ All tests completed successfully  
✓ No memory leaks detected  
✓ System remained stable under load  
✓ EventChains shows competitive performance, especially on larger workloads