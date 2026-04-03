# cat (C++)

Simple implementation of the `cat` command.

## Features

- Reads from file(s) or STDIN
- `-n` → show line numbers
- `-b` → show line numbers for non-empty lines only
- `-e` → mark end of line with `$`

If both `-n` and `-b` are given, `-b` is used.

## How it works

- Reads input byte-by-byte using `read()`
- Writes using a safe `write()` wrapper
- Tracks line state to apply flags correctly

## Usage

./cat file.txt  
./cat -n file.txt  
./cat -b file.txt  
./cat -e file.txt  

## Notes

- Supports multiple files
- `-` means STDIN
- Handles read/write errors
