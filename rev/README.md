# rev (C++)

Reverses each line of input.

## Features

- Reverses characters line by line
- Works with files and STDIN

## How it works

- Reads input using `read()` or `mmap()`
- Builds each line in memory
- Reverses using `std::reverse`
- Writes output after each line

## Usage

./rev file.txt  

## Notes

- Handles last line without newline
- Uses `mmap()` for regular files
- Falls back to `read()` otherwise
- Handles read/write errors
