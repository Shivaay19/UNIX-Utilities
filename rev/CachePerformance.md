## Performance Analysis (Cachegrind)

This implementation was compared against the system `rev` utility using Valgrind's Cachegrind to analyze performance characteristics.

### Setup

Both programs were executed on the same input file:

```bash
valgrind --tool=cachegrind --cache-sim=yes <command>
```

---

### Results

| Metric       | This Implementation | GNU `rev`  |
| ------------ | ------------------- | ---------- |
| I refs       | ~2,850,000          | ~1,960,000 |
| D refs       | ~1,348,000          | ~952,000   |
| I1 miss rate | ~0.07%              | ~0.09%     |
| D1 miss rate | ~1.5%               | ~0.6%      |

---

### Key Observations

* This implementation executes **~1.4× more instructions** than GNU `rev`.
* It performs **~1.4× more data accesses**.
* Cache performance is already strong in both implementations, with low miss rates overall.
* GNU `rev` shows **better data cache efficiency** (lower D1 miss rate).

---

### Reasons for the Gap

1. **Extra Memory Passes / Buffer Handling**
   Reversing lines typically requires buffering input and then writing it back in reverse. Less efficient buffer handling can increase memory traffic.

2. **Higher Per-byte Overhead**
   Additional checks or logic inside the processing loop increase instruction count.

---

### Takeaway

Unlike simpler utilities (`cat`, `head`), `rev` inherently requires more work (due to reversing), so the gap is smaller.