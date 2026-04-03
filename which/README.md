# which (C)

Finds the full path of a command.

## Features

- Searches command in PATH
- Supports direct paths (with `/`)

## How it works

- If input contains `/`, checks directly using `access()`
- Otherwise:
  - Reads PATH using `getenv`
  - Splits directories using `strtok`
  - Checks each path using `access(X_OK)`

## Usage

./which ls  
./which ./a.out  

## Notes

- Prints first match only
- Prints "not found" if not found
- Requires exactly one argument
