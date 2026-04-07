## Performance Analysis (Cachegrind)

This implementation was compared against the system `rev` utility using Valgrind's Cachegrind with cache parameters configured to match the host machine.

### Setup

Both programs were executed on the same input file.

Cache configuration used:

* I1: 128 KB, 8-way associative, 128B line size
* D1: 64 KB, 8-way associative, 128B line size

```bash id="k1n9lp"
valgrind --tool=cachegrind \
  --I1=131072,8,128 \
  --D1=65536,8,128 \
  --cache-sim=yes <command>
```

---

### Results

| Metric       | This Implementation | GNU `rev`  |
| ------------ | ------------------- | ---------- |
| I refs       | ~2,850,000          | ~1,960,000 |
| D refs       | ~1,348,000          | ~952,000   |
| I1 miss rate | ~0.03%              | ~0.05%     |
| D1 miss rate | ~0.7%               | ~0.2%      |

---

### Key Observations

* This implementation executes **~1.4× more instructions** than GNU `rev`.
* It performs **~1.4× more data accesses**.
* Both implementations exhibit **very low cache miss rates**, indicating good locality.
* GNU `rev` achieves **better data cache efficiency** (lower D1 miss rate).

---

### Reasons for the Gap

1. **Higher Memory Traffic**
   This implementation performs more reads and writes while reversing lines, increasing overall data references.

2. **Per-byte Processing Overhead**
   Additional logic inside tight loops increases instruction count.

3. **Less Efficient Memory Access Patterns**
   Reversing data may involve accessing memory in a way that is less cache-friendly compared to the system implementation.

4. **Extra Control Flow Overhead**
   Slightly more branching or bookkeeping during reversal contributes to increased instruction count.

---

### Takeaway

Unlike simpler utilities (`cat`, `head`), `rev` inherently requires more work due to reversing data, so the performance gap is smaller.