# JitFromScratch / jit-function-calls / implicit-extern-c 

[![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=jit-function-calls/implicit-extern-c)](https://travis-ci.org/weliveindetail/JitFromScratch)

This branch shows how to declare a function as `extern "C"` and call it from JITed code. For `extern "C"` functions only the system mangling scheme is applied, so they can easily be resolved by their raw name from the symbol table of the host executable. This happens automatically in the ObjectLayer while linking.

The `mangle_ref` scripts shows how to examine object files for function symbols.

## Build and Run

Tested on Linux Mint 18, Mac OS X 10.12 and Windows 10. For detailed instructions please refer to [Building a JIT from scratch](https://weliveindetail.github.io/blog/post/2017/07/18/building-a-jit-from-scratch.html).

### Linux / OSX
```
$ cd JitFromScratch/mangle-demo
$ ./mangle_ref_unix 
                 U _customIntAllocator
0000000000000000 T _main
$ cd ../../JitFromScratch-build
$ cmake -DLLVM_DIR=/path/to/llvm40-build/lib/cmake/llvm -DCMAKE_BUILD_TYPE=Debug ../JitFromScratch
$ cmake --build .
$ ./JitFromScratch
Integer Distances: 3, 0, 3
```

### Windows
```
C:\JitFromScratch>cd JitFromScratch\mangle-demo
C:\JitFromScratch\JitFromScratch\mangle-demo>mangle_ref_win
Microsoft (R) COFF/PE Dumper Version 14.10.25019.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file mangle_ref_win.obj

File Type: COFF OBJECT

COFF SYMBOL TABLE
000 00000000 SECT1  notype       Static       | .text
    Section length    9, #relocs    0, #linenums    0, checksum 12D998E4
002 00000000 SECT2  notype       Static       | .data
    Section length    0, #relocs    0, #linenums    0, checksum        0
004 00000000 SECT3  notype       Static       | .bss
    Section length    0, #relocs    0, #linenums    0, checksum        0
006 00000000 SECT1  notype ()    External     | customIntAllocator

String Table Size = 0x17 bytes

  Summary

           0 .bss
           0 .data
           9 .text

C:\JitFromScratch\JitFromScratch\mangle-demo>cd ..\..\JitFromScratch-build
C:\JitFromScratch\JitFromScratch-build>cmake -DLLVM_DIR=/path/to/llvm40-build/lib/cmake/llvm ../JitFromScratch
C:\JitFromScratch\JitFromScratch-build>cmake --build .
C:\JitFromScratch\JitFromScratch-build>JitFromScratch.exe
Integer Distances: 3, 0, 3
```
