# UNIX Utilities (C/C++)

Reimplementation of core UNIX command-line utilities in C/C++ with a focus on low-level system programming and performance.

---

## Commands

### cat

Display file contents with additional formatting options.

**Features**

* `-n` → show line numbers
* `-b` → show line numbers for non-empty lines
* `-e` → mark end of line with `$`

**Usage**

```bash
./cat file.txt
./cat -n file.txt
./cat -b file.txt
./cat -e file.txt
```

---

### head

Output the first part of files.

**Features**

* Default: first 10 lines
* `-n <num>` → first N lines
* `-c <num>` → first N bytes
* Supports multiple files with headers

**Usage**

```bash
./head file.txt
./head -n 5 file.txt
./head -c 20 file.txt
```

---

### rev

Reverse each line of input.

**Features**

* Reverses characters line by line
* Works with files and STDIN

**Usage**

```bash
./rev file.txt
```

---

### which

Locate an executable in PATH.

**Features**

* Searches PATH directories
* Supports direct paths

**Usage**

```bash
./which ls
./which ./a.out
```

---

## Build

Compile using `gcc` / `g++`:

```bash
g++ cat.cpp -o cat
g++ head.cpp -o head
g++ rev.cpp -o rev
gcc which.c -o which
```

---

## Key Design Decisions

### System Calls

* Uses `read()`, `write()`, `open()`, `access()`
* Avoids high-level I/O for better control

### Reliable Output

* Custom `writeAllBytes()` ensures full writes
* Handles partial writes and interruptions (`EINTR`)

### Efficient File Handling

* `read()` for streams and STDIN
* `mmap()` for regular files (head, rev)

### Argument Parsing

* Uses `strtol` for numeric arguments
* Strict validation for invalid input

### STDIN Support

* All commands support `-` as input

---

## Error Handling

* Handles:

  * file open failures
  * invalid arguments
  * read/write errors
* Proper handling of interrupted system calls

---

## Project Structure

```bash
.
├── cat/
├── head/
├── rev/
├── which/
└── README.md
```

---

## Limitations

* Not fully GNU-compatible
* Limited flag support
* No advanced optimizations
