## Performance Analysis (Cachegrind)

This "wc-io-optimized.cpp" implementation was compared against the system `wc` utility using Valgrind's Cachegrind to evaluate performance.

### Setup

Both programs were executed on the same input file:

```bash
valgrind --tool=cachegrind --cache-sim=yes <command>
```

---

### Results

| Metric       | This Implementation | GNU `wc` |
| ------------ | ------------------- | -------- |
| I refs       | ~1,550,000          | ~200,000 |
| D refs       | ~630,000            | ~76,000  |
| I1 miss rate | ~0.12%              | ~0.79%   |
| D1 miss rate | ~3.1%               | ~4.2%    |

---

### Key Observations

* This implementation executes **~7–8× more instructions** than GNU `wc`.
* It performs **~8× more data accesses**.
* Despite this, it achieves **better cache locality** (lower miss rates).
* Overall performance is dominated by the higher instruction count rather than cache behavior.

---

### Reasons for the Gap

1. **Per-byte Processing Overhead**
   Each character is processed individually with multiple condition checks, increasing instruction count.

2. **Branch-heavy Inner Loop**
   Frequent branching (word detection, newline handling, etc.) adds overhead in tight loops.

---

### Takeaway

The primary bottleneck is **instruction count**, not cache efficiency.

Even with good cache behavior, executing significantly more instructions leads to slower performance.

> Reducing the amount of work per byte is more important than optimizing cache usage alone.