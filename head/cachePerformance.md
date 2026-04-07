## Performance Analysis (Cachegrind)

This implementation was compared against the system `head` utility using Valgrind's Cachegrind with cache parameters configured to match the host machine.

### Setup

Both programs were executed on the same input file with output redirected to `/dev/null`.

Cache configuration used:

* I1: 128 KB, 8-way associative, 128B line size
* D1: 64 KB, 8-way associative, 128B line size

```bash id="y0b7k2"
valgrind --tool=cachegrind \
  --I1=131072,8,128 \
  --D1=65536,8,128 \
  --cache-sim=yes <command> >> /dev/null
```

---

### Results

| Metric       | This Implementation | GNU `head` |
| ------------ | ------------------- | ---------- |
| I refs       | ~1,588,000          | ~182,000   |
| D refs       | ~642,000            | ~69,000    |
| I1 miss rate | ~0.06%              | ~0.43%     |
| D1 miss rate | ~1.5%               | ~2.0%      |

---

### Key Observations

* This implementation executes **~8–9× more instructions** than GNU `head`.
* It performs **~9× more data accesses**.
* It achieves **better cache locality** (lower instruction and data cache miss rates).
* Despite improved cache behavior, the implementation is significantly slower due to higher total work.

---

### Reasons for the Gap

1. **Excessive Work Per Byte**
   The implementation processes input byte-by-byte with additional logic, while `head` only needs to detect newline characters and stop early.

2. **Lack of Early Termination Efficiency**
   GNU `head` stops processing as soon as the required number of lines is read, minimizing total work.

3. **Higher Loop Overhead**
   More instructions are executed per iteration due to additional checks and control flow.

4. **Instruction Count Dominates**
   Even with better cache performance, the large difference in instruction count leads to worse overall performance.

---

### Takeaway

The dominant factor in this comparison is **instruction count**, not cache performance.

Even under realistic cache parameters, executing significantly more instructions outweighs the benefits of improved cache locality.