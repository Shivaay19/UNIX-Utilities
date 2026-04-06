## Performance Analysis (Cachegrind)

This implementation was compared against the system `which` utility using Valgrind's Cachegrind to analyze performance.

### Setup

Both programs were executed with simple commands:

```bash
valgrind --tool=cachegrind --cache-sim=yes which <command>
valgrind --tool=cachegrind --cache-sim=yes ./which-baseline <command>
```

---

### Results

| Metric       | This Implementation | System `which` |
| ------------ | ------------------- | -------------- |
| I refs       | ~114,000            | ~536,000       |
| D refs       | ~46,000             | ~236,000       |
| I1 miss rate | ~1.01%              | ~0.93%         |
| D1 miss rate | ~5.1%               | ~1.6%          |

---

### Key Observations

* This implementation executes **~4–5× fewer instructions** than the system `which`.
* It performs **~5× fewer data accesses**.
* However, it has **worse cache locality**, especially on the data side (higher D1 miss rate).

---

### Reasons for the Difference

1. **Minimal Feature Set**
   This implementation strictly searches the `PATH` for an executable using `access()`. It does not handle shell-specific features such as aliases, built-ins, or extended lookup behavior, which reduces overall work.

2. **Straightforward PATH Traversal**
   The use of `strtok` to iterate through `PATH` and a simple `snprintf + access` check results in a direct and predictable control flow with fewer instructions.

3. **Lower Abstraction Overhead**
   The implementation avoids additional layers such as helper utilities, configuration handling, or extended validation logic, which are typically present in system utilities.

4. **Higher Cache Miss Rate Due to Memory Patterns**
   Operations like `strdup`, `strtok`, and repeated string construction (`snprintf`) introduce scattered memory accesses, leading to higher cache miss rates compared to more optimized implementations.

---

### Takeaway

Unlike other utilities (`wc`, `cat`, `head`), this implementation is:

> More instruction-efficient but less cache-efficient.
