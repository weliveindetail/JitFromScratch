# JitFromScratch

[![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm50/jit-debug/generic-ir-debug-info/linux)](https://travis-ci.org/weliveindetail/JitFromScratch)

**This is the LLVM 5.0 compatible version of the examples. Please find the latest version [here](https://github.com/weliveindetail/JitFromScratch).**

### Build Instructions
```
mkdir build-debug
cd build-debug
cmake -DLLVM_DIR=/path/to/build-llvm40/lib/cmake/llvm -DCMAKE_BUILD_TYPE=Debug ../JitFromScratch
cmake --build .
```

