# OpenMP on macOS with Clang

This project demonstrates using OpenMP with Clang on macOS for parallel computing.

## Prerequisites

Install LLVM with OpenMP support via Homebrew:
```bash
brew install llvm libomp
```

## Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Run

```bash
./array_multiply
```

## Example Output

```
Array size: 100000000 elements
Available threads: 8

Without OpenMP: 245 ms
With OpenMP:    35 ms

Speedup: 7.0x
```
