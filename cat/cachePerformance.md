## Performance Analysis (Cachegrind)

This implementation was compared against the system `cat` utility using Valgrind's Cachegrind with cache parameters configured to match the host machine.

### Setup

Both programs were executed on the same input file.

Cache configuration used:

* I1: 128 KB, 8-way associative, 128B line size
* D1: 64 KB, 8-way associative, 128B line size

```bash id="9y4z4q"
valgrind --tool=cachegrind \
  --I1=131072,8,128 \
  --D1=65536,8,128 \
  --cache-sim=yes <command>
```

---

### Results

| Metric       | This Implementation | GNU `cat` |
| ------------ | ------------------- | --------- |
| I refs       | ~2,670,000          | ~180,000  |
| D refs       | ~1,128,000          | ~69,000   |
| I1 miss rate | ~0.04%              | ~0.43%    |
| D1 miss rate | ~0.8%               | ~2.0%     |

---

### Key Observations

* This implementation executes **~15× more instructions** than GNU `cat`.
* It performs **~16× more data accesses**.
* It achieves **significantly better cache locality** (lower miss rates on both instruction and data caches).
* Despite better cache performance, it is slower due to much higher total work.

---

### Reasons for the Gap

1. **Per-byte Processing Overhead**
   The implementation processes input byte-by-byte, leading to a large number of loop iterations and instructions.

2. **Excessive Loop Overhead**
   Even minimal logic inside a tight loop becomes expensive when executed millions of times.


4. **Instruction Efficiency vs Cache Efficiency Trade-off**
   This implementation is cache-friendly but performs significantly more instructions per byte.

---

### Takeaway

The dominant factor here is **instruction count**, not cache performance.

Even with improved cache locality under realistic cache parameters, the large gap in instruction count leads to significantly worse performance.