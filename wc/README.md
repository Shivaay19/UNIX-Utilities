### wc implementations

1. wc-baseline
   - Supports -w -l -c
   - Multiple file arguments

2. wc-io-optimized
   - read() and mmap() based implementations
   - Adds support for -L (max line length)

3. wc-flag-bitmask
   - Uses bitmask-based flag handling
   - Cleaner internal state representation