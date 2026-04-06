## Performance Analysis (Cachegrind)

This implementation was compared against the system `cat` utility using Valgrind's Cachegrind to understand performance differences.

### Setup

Both programs were executed on the same input file:

```bash
valgrind --tool=cachegrind --cache-sim=yes <command>
```

---

### Results

| Metric       | This Implementation | GNU `cat` |
| ------------ | ------------------- | --------- |
| I refs       | ~2,670,000          | ~180,000  |
| D refs       | ~1,128,000          | ~69,000   |
| I1 miss rate | ~0.07%              | ~0.79%    |
| D1 miss rate | ~1.7%               | ~4.4%     |

---

### Key Observations

* This implementation executes **~15× more instructions** than GNU `cat`.
* It also performs **~16× more data accesses**.
* Despite significantly **better cache locality** (lower miss rates), it is much slower due to the sheer volume of instructions executed.

---

### Reasons for the Gap

1. **Per-byte Processing Overhead**
   The implementation processes input byte-by-byte, leading to a large number of loop iterations and instructions.

2. **Excessive Loop and Branching Overhead**
   Even minimal logic inside a tight loop becomes expensive when executed millions of times.

3. **Lack of Zero-copy or Bulk Transfer**
   GNU `cat` is highly optimized and often uses large block transfers or kernel-assisted mechanisms, minimizing CPU involvement.

---

### Takeaway

The dominant factor here is **instruction count**, not cache performance.

Even though this implementation is cache-efficient, it performs significantly more work per byte.