## Performance Analysis (Cachegrind)

To evaluate the efficiency of this implementation, I compared it against the system `head` utility using Valgrind's Cachegrind tool.

### Setup

Both programs were run on the same input file and analyzed using:

```bash
valgrind --tool=cachegrind --cache-sim=yes <command>
```

---

### Results

#### Baseline Comparison

| Metric       | This Implementation | GNU `head` |
| ------------ | ------------------- | ---------- |
| I refs       | ~1,580,000          | ~188,000   |
| D refs       | ~638,000            | ~72,000    |
| I1 miss rate | ~0.12%              | ~0.76%     |
| D1 miss rate | ~3.1%               | ~4.2%      |

#### With `-n 20` (similar workload)

| Metric | This Implementation | GNU `head` |
| ------ | ------------------- | ---------- |
| I refs | ~1,570,000          | ~373,000   |
| D refs | ~636,000            | ~131,000   |

---

### Key Observations

* This implementation executes **~4–8× more instructions** than GNU `head`.
* Despite having **better cache locality** (lower miss rates), it is still slower due to higher total work.
* GNU `head` performs fewer memory accesses and significantly fewer instructions overall.