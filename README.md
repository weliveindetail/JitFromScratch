# JitFromScratch / jit-optimization

[![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=jit-optimization)](https://travis-ci.org/weliveindetail/JitFromScratch)

This branch shows how to apply optimizations to JITed code in a ORC-based compiler and demonstrates how instruction ordering affects automatic vectorization.

## Build and Run

Tested on Linux Mint 18, Mac OS X 10.12 and Windows 10. For detailed instructions please refer to [Building a JIT from scratch](https://weliveindetail.github.io/blog/post/2017/07/18/building-a-jit-from-scratch.html).

```
$ cmake -DLLVM_DIR=/path/to/build-llvm40/lib/cmake/llvm -DCMAKE_BUILD_TYPE=Debug ../JitFromScratch
$ cmake --build .
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
  %3 = getelementptr inbounds i32, i32* %0, i32 0
  %4 = getelementptr inbounds i32, i32* %x, i32 1
  %5 = getelementptr inbounds i32, i32* %y, i32 1
  %6 = getelementptr inbounds i32, i32* %0, i32 1
  %7 = getelementptr inbounds i32, i32* %x, i32 2
  %8 = getelementptr inbounds i32, i32* %y, i32 2
  %9 = getelementptr inbounds i32, i32* %0, i32 2
  %10 = load i32, i32* %1
  %11 = load i32, i32* %2
  %12 = sub i32 %10, %11
  %13 = call i32 @abs(i32 %12)
  %14 = load i32, i32* %4
  %15 = load i32, i32* %5
  %16 = sub i32 %14, %15
  %17 = call i32 @abs(i32 %16)
  %18 = load i32, i32* %7
  %19 = load i32, i32* %8
  %20 = sub i32 %18, %19
  %21 = call i32 @abs(i32 %20)
  store i32 %13, i32* %3
  store i32 %17, i32* %6
  store i32 %21, i32* %9
  ret i32* %0
}

declare i32* @customIntAllocator(i32)

declare i32 @abs(i32)


Optimized module:

; ModuleID = 'JitFromScratch'
source_filename = "JitFromScratch"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"

define i32* @integerDistance(i32* nocapture readonly %x, i32* nocapture readonly %y) local_unnamed_addr {
entry:
  %0 = tail call i32* @customIntAllocator(i32 3)
  %1 = getelementptr inbounds i32, i32* %x, i64 2
  %2 = getelementptr inbounds i32, i32* %y, i64 2
  %3 = getelementptr inbounds i32, i32* %0, i64 2
  %4 = bitcast i32* %x to <2 x i32>*
  %5 = bitcast i32* %y to <2 x i32>*
  %6 = load <2 x i32>, <2 x i32>* %4, align 4
  %7 = load <2 x i32>, <2 x i32>* %5, align 4
  %8 = sub <2 x i32> %6, %7
  %ispos = icmp sgt <2 x i32> %8, <i32 -1, i32 -1>
  %neg = sub <2 x i32> zeroinitializer, %8
  %9 = select <2 x i1> %ispos, <2 x i32> %8, <2 x i32> %neg
  %10 = load i32, i32* %1, align 4
  %11 = load i32, i32* %2, align 4
  %12 = sub i32 %10, %11
  %ispos3 = icmp sgt i32 %12, -1
  %neg4 = sub i32 0, %12
  %13 = select i1 %ispos3, i32 %12, i32 %neg4
  %14 = bitcast i32* %0 to <2 x i32>*
  store <2 x i32> %9, <2 x i32>* %14, align 4
  store i32 %13, i32* %3, align 4
  ret i32* %0
}

declare i32* @customIntAllocator(i32) local_unnamed_addr


Integer Distances: 3, 0, 3
```
