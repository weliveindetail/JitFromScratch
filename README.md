# JitFromScratch / jit-function-calls / explicit

[![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm50/jit-function-calls/explicit)](https://travis-ci.org/weliveindetail/JitFromScratch)

This branch shows how to call external functions from JITed code through a local symbol table implemented in the `GlobalMappingLayer`. 

The good thing with this approach is that you have explicit control over symbol mappings without the need of include hacks.

The bad thing is that the `GlobalMappingLayer` inevitably falls back to its base layer if it cannot provide the symbol in question. This behavior undermines separation of concerns between `findSymbolInJITedCode()` and `findSymbolInHostProcess()`. While the latter **is** supposed to resolve explicitly mapped symbols (like `customIntAllocator`) and therefore calls `MappingLayer.findSymbol()`, it **is not** supposed to resolve JIT compiled ones (like `integerDistance`). By calling `MappingLayer.findSymbol("integerDistance", false)`, however, it will provide to the correct address of the function in JIT compiled code, as the call silently forwards to `CompileLayer.findSymbol()` and `ObjectLayer.findSymbol()`. It doesn't affect the example, but it's good to keep in mind!
## Build and Run

Tested on Linux Mint 18, Mac OS X 10.12 and Windows 10. For detailed instructions please refer to [Building a JIT from scratch](https://weliveindetail.github.io/blog/post/2017/07/18/building-a-jit-from-scratch.html).

```
$ cmake -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR=/path/to/llvm50-build/lib/cmake/llvm ../JitFromScratch
$ cmake --build .
$ ./JitFromScratch -debug -debug-only jitfromscratch
Found symbol 'integerDistance' by calling 'findSymbolInHostProcess()'.
This is a side effect of using the GlobalMappingLayer.

Integer Distances: 3, 0, 3
```
