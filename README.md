# JitFromScratch / jit-debug / llvm-debug-dumps

[![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=jit-debug/llvm-debug-dumps)](https://travis-ci.org/weliveindetail/JitFromScratch)

This branch shows how to adopt the LLVM approach for printing *per-component* debug dumps, controlled via command line arguments `-debug` and `-debug-only`.

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
```

You can also dump details of other LLVM components like [RuntimeDyld](https://github.com/llvm-mirror/llvm/blob/master/lib/ExecutionEngine/RuntimeDyld/RuntimeDyld.cpp#L29), which can be extremely useful when tracking down bugs. Example output for the **[jit-basics branch](https://github.com/weliveindetail/JitFromScratch/tree/jit-basics)**:
```
$ ./JitFromScratch -debug -debug-only dyld
Parse symbols:
emitSection SectionID: 0 Name: __text obj addr: 0x7f912e01bdd0 new addr: 0x10825b000 DataSize: 98 StubBufSize: 16 Allocate: 114
  Type: 4 Name: _integerDistance SID: 0 Offset: 0x0 flags: 66
Parse relocations:
  SectionID: 0
emitSection SectionID: 1 Name: __compact_unwind obj addr: 0x7f912e01be38 new addr: 0x10825c000 DataSize: 32 StubBufSize: 8 Allocate: 40
  SectionID: 1
emitSection SectionID: 2 Name: __eh_frame obj addr: 0x7f912e01be58 new addr: 0x10825c028 DataSize: 72 StubBufSize: 0 Allocate: 72
----- Contents of section __text before relocations -----
0x000000010825b000: 41 57 41 56 41 54 53 50 49 89 f6 49 89 ff 48 b8
0x000000010825b010: 00 00 00 00 00 00 00 00 bf 03 00 00 00 ff d0 48
0x000000010825b020: 89 c3 41 8b 3f 41 2b 3e 49 bc 00 00 00 00 00 00
0x000000010825b030: 00 00 41 ff d4 89 03 41 8b 7f 04 41 2b 7e 04 41
0x000000010825b040: ff d4 89 43 04 41 8b 7f 08 41 2b 7e 08 41 ff d4
0x000000010825b050: 89 43 08 48 89 d8 48 83 c4 08 5b 41 5c 41 5e 41
0x000000010825b060: 5f c3
----- Contents of section __compact_unwind before relocations -----
0x000000010825c000: 00 00 00 00 00 00 00 00 62 00 00 00 00 00 00 04
0x000000010825c010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
----- Contents of section __eh_frame before relocations -----
0x000000010825c020:                         14 00 00 00 00 00 00 00
0x000000010825c030: 01 7a 52 00 01 78 10 01 10 0c 07 08 90 01 00 00
0x000000010825c040: 2c 00 00 00 1c 00 00 00 58 ff ff ff ff ff ff ff
0x000000010825c050: 62 00 00 00 00 00 00 00 00 42 0e 10 42 0e 18 42
0x000000010825c060: 0e 20 41 0e 28 41 0e 30 83 05 8c 04 8e 03 8f 02
Resolving relocations Name: _abs  0x7fffeac57e1a
resolveRelocation Section: 0 LocalAddress: 0x10825b02a FinalAddress: 0x000000010825b02a Value: 0x00007fffeac57e1a Addend: 0 isPCRel: 0 MachoType: 0 Size: 8
Resolving relocations Name: _customIntAllocator 0x104729980
resolveRelocation Section: 0 LocalAddress: 0x10825b010 FinalAddress: 0x000000010825b010 Value: 0x0000000104729980 Addend: 0 isPCRel: 0 MachoType: 0 Size: 8
Resolving relocations Section #0  0x10825b000
resolveRelocation Section: 1 LocalAddress: 0x10825c000 FinalAddress: 0x000000010825c000 Value: 0x000000010825b000 Addend: 0 isPCRel: 0 MachoType: 0 Size: 8
----- Contents of section __text after relocations -----
0x000000010825b000: 41 57 41 56 41 54 53 50 49 89 f6 49 89 ff 48 b8
0x000000010825b010: 80 99 72 04 01 00 00 00 bf 03 00 00 00 ff d0 48
0x000000010825b020: 89 c3 41 8b 3f 41 2b 3e 49 bc 1a 7e c5 ea ff 7f
0x000000010825b030: 00 00 41 ff d4 89 03 41 8b 7f 04 41 2b 7e 04 41
0x000000010825b040: ff d4 89 43 04 41 8b 7f 08 41 2b 7e 08 41 ff d4
0x000000010825b050: 89 43 08 48 89 d8 48 83 c4 08 5b 41 5c 41 5e 41
0x000000010825b060: 5f c3
----- Contents of section __compact_unwind after relocations -----
0x000000010825c000: 00 b0 25 08 01 00 00 00 62 00 00 00 00 00 00 04
0x000000010825c010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
----- Contents of section __eh_frame after relocations -----
0x000000010825c020:                         14 00 00 00 00 00 00 00
0x000000010825c030: 01 7a 52 00 01 78 10 01 10 0c 07 08 90 01 00 00
0x000000010825c040: 2c 00 00 00 1c 00 00 00 58 ff ff ff ff ff ff ff
0x000000010825c050: 62 00 00 00 00 00 00 00 00 42 0e 10 42 0e 18 42
0x000000010825c060: 0e 20 41 0e 28 41 0e 30 83 05 8c 04 8e 03 8f 02
Processing FDE: Delta for text: 4000, Delta for EH: 0
Processing FDE: Delta for text: 4000, Delta for EH: 0
Integer Distances: 3, 0, 3
```