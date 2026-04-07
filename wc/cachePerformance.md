## Performance Analysis (Cachegrind)

This implementation was compared against the system `wc` utility using Valgrind's Cachegrind with cache parameters configured to match the host machine.

### Setup

Both programs were executed on the same input file.

Cache configuration used:

* I1: 128 KB, 8-way associative, 128B line size
* D1: 64 KB, 8-way associative, 128B line size

```bash id="r3p0vh"
valgrind --tool=cachegrind \
  --I1=131072,8,128 \
  --D1=65536,8,128 \
  --cache-sim=yes <command>
```

---

### Results

| Metric       | This Implementation | GNU `wc` |
| ------------ | ------------------- | -------- |
| I refs       | ~2,037,000          | ~537,000 |
| D refs       | ~882,000            | ~163,000 |
| I1 miss rate | ~0.05%              | ~0.16%   |
| D1 miss rate | ~1.1%               | ~1.0%    |

---

### Key Observations

* This implementation executes **~3.8× more instructions** than GNU `wc`.
* It performs **~5× more data accesses**.
* Instruction cache performance is **better** in this implementation (lower I1 miss rate).
* Data cache performance is **comparable** between both implementations.

---

### Reasons for the Gap

1. **Higher Per-byte Processing Cost**
   Each character is processed with multiple checks (whitespace detection, word tracking, line tracking), increasing instruction count.

2. **Branch-heavy Inner Loop**
   Frequent branching inside the tight loop leads to more executed instructions.

3. **More Work Per Input Byte**
   Even with efficient I/O (read/mmap), the computation performed per byte is higher than in the system implementation.

4. **Instruction Efficiency Dominates**
   Since cache behavior is already good in both implementations, the primary difference comes from total instructions executed.

---

### Takeaway

The dominant factor in this comparison is **instruction count**, not cache performance.

Even with similar cache behavior under realistic cache parameters, executing significantly more instructions results in slower performance.