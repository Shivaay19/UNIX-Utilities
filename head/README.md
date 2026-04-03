# head (C++)

Implementation of the `head` command.

## Features

- Default: first 10 lines
- `-n <num>` → first N lines
- `-c <num>` → first N bytes
- Supports multiple files
- Shows headers for multiple files

If both `-n` and `-c` are used, `-c` is used.

## How it works

- Parses arguments using `strtol`
- Line mode: stops after N newlines
- Byte mode: stops after N bytes
- Uses `read()` or `mmap()` depending on file type

## Usage

./head file.txt  
./head -n 5 file.txt  
./head -c 20 file.txt  

## Notes

- `-` means STDIN
- Validates arguments strictly
- Uses `mmap()` for regular files
