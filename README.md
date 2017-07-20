# JitFromScratch / jit-basics

[![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=jit-basics)](https://travis-ci.org/weliveindetail/JitFromScratch)

This branch establishes a common code base for all further examples by demonstrating how to compile the code for a simple function at runtime:

```
template <size_t sizeOfArray>
int *integerDistances(const int (&x)[sizeOfArray], int *y) {
  unsigned items = arrayElements(x);
  int *results = customIntAllocator(items);

  for (unsigned i = 0; i < items; i++) {
    results[i] = abs(x[i] - y[i]);
  }

  return results;
}
```

## Build and Run

Tested on Linux Mint 18, Mac OS X 10.12 and Windows 10. For detailed instructions please refer to [Building a JIT from scratch](https://weliveindetail.github.io/blog/post/2017/07/18/building-a-jit-from-scratch.html).

```
$ cmake -DLLVM_DIR=/path/to/build-llvm40/lib/cmake/llvm -DCMAKE_BUILD_TYPE=Debug ../JitFromScratch
$ cmake --build .
$ ./JitFromScratch
Integer Distances: 3, 0, 3
$ ./JitFromScratch -debug -debug-only=jitfromscratch
Submit LLVM module:

; ModuleID = 'JitFromScratch'
source_filename = "JitFromScratch"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"

define i32* @integerDistance(i32* %x, i32* %y) {
entry:
  %0 = call i32* @customIntAllocator(i32 3)
  %1 = getelementptr inbounds i32, i32* %x, i32 0
  %2 = getelementptr inbounds i32, i32* %y, i32 0
  %3 = load i32, i32* %1
  %4 = load i32, i32* %2
  %5 = sub i32 %3, %4
  %6 = call i32 @abs(i32 %5)
  %7 = getelementptr inbounds i32, i32* %0, i32 0
  store i32 %6, i32* %7
  %8 = getelementptr inbounds i32, i32* %x, i32 1
  %9 = getelementptr inbounds i32, i32* %y, i32 1
  %10 = load i32, i32* %8
  %11 = load i32, i32* %9
  %12 = sub i32 %10, %11
  %13 = call i32 @abs(i32 %12)
  %14 = getelementptr inbounds i32, i32* %0, i32 1
  store i32 %13, i32* %14
  %15 = getelementptr inbounds i32, i32* %x, i32 2
  %16 = getelementptr inbounds i32, i32* %y, i32 2
  %17 = load i32, i32* %15
  %18 = load i32, i32* %16
  %19 = sub i32 %17, %18
  %20 = call i32 @abs(i32 %19)
  %21 = getelementptr inbounds i32, i32* %0, i32 2
  store i32 %20, i32* %21
  ret i32* %0
}

declare i32* @customIntAllocator(i32)

declare i32 @abs(i32)


Integer Distances: 3, 0, 3
```
