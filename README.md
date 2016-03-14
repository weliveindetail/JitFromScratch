# JitFromScratch
Example project from my talk in the LLVM Social and C++ User Group Berlin

[![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=jit-basics)](https://travis-ci.org/weliveindetail/JitFromScratch)

### Build Instructions
```
mkdir build-debug
cd build-debug
cmake -DLLVM_DIR=/path/to/build-llvm40/lib/cmake/llvm -DCMAKE_BUILD_TYPE=Debug ../JitFromScratch
cmake --build .
```

