# JitFromScratch / jit-debug / gdb-interface

[![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm50/jit-debug/gdb-interface)](https://travis-ci.org/weliveindetail/JitFromScratch)

This branch shows how to implement the [GDB JIT Interface](https://sourceware.org/gdb/onlinedocs/gdb/JIT-Interface.html) for a ORC-based JIT compiler.

## Build and Run

### Linux

JIT debugging is expected to work on Linux with the official LLVM Release 5.0.

```
user@linux ~/JitFromScratch/build $ cmake -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR=/path/to/build-llvm50/lib/cmake/llvm ../JitFromScratch
user@linux ~/JitFromScratch/build $ cmake --build .
user@linux ~/JitFromScratch/build $ lldb -- JitFromScratch 
(lldb) target create "JitFromScratch"
Current executable set to 'JitFromScratch' (x86_64).
(lldb) log enable lldb jit
(lldb) b integerDistance
Breakpoint 1: no locations (pending).
WARNING:  Unable to resolve breakpoint to any actual locations.
(lldb) r
JITLoaderGDB::SetJITBreakpoint looking for JIT register hook
JITLoaderGDB::SetJITBreakpoint setting JIT breakpoint
Process 9232 launched: '~/JitFromScratch/build/JitFromScratch' (x86_64)
JITLoaderGDB::JITDebugBreakpointHit hit JIT breakpoint
JITLoaderGDB::ReadJITDescriptorImpl registering JIT entry at 0x2be0c20 (1088 bytes)
1 location added to breakpoint 1
Process 9232 stopped
* thread #1, name = 'JitFromScratch', stop reason = breakpoint 1.1
    frame #0: 0x00007ffff7ff5000 JIT(0x2be0c20)`integerDistance
JIT(0x2be0c20)`:
->  0x7ffff7ff5000 <+0>: pushq  %r15
    0x7ffff7ff5002 <+2>: pushq  %r14
    0x7ffff7ff5004 <+4>: pushq  %r12
    0x7ffff7ff5006 <+6>: pushq  %rbx
(lldb) bt
* thread #1, name = 'JitFromScratch', stop reason = breakpoint 1.1
  * frame #0: 0x00007ffff7ff5000 JIT(0x2be0c20)`integerDistance
    frame #1: JitFromScratch`std::_Function_handler<int* (int*, int*), int* (*)(int*, int*)>::_M_invoke(__functor=0x00007fffffffd9c0, __args=0x00007fffffffd8b0, __args=0x00007fffffffd8a8) at functional:1856
    frame #2: JitFromScratch`std::function<int* (int*, int*)>::operator(this=0x00007fffffffd9c0, __args=0x00007fffffffda48, __args=0x00007fffffffda38)(int*, int*) const at functional:2267
    frame #3: JitFromScratch`main(argc=1, argv=0x00007fffffffdf18) at JitFromScratch.cpp:129
    frame #4: libc.so.6`__libc_start_main(main=(JitFromScratch`main at JitFromScratch.cpp:98), argc=1, argv=0x00007fffffffdf18, init=<unavailable>, fini=<unavailable>, rtld_fini=<unavailable>, stack_end=0x00007fffffffdf08) at libc-start.c:291
    frame #5: 0x0000000000788559 JitFromScratch`_start + 41
(lldb) c
Process 9232 resuming
Integer Distances: 3, 0, 3

JITLoaderGDB::JITDebugBreakpointHit hit JIT breakpoint
JITLoaderGDB::ReadJITDescriptorImpl unregistering JIT entry at 0x2be0c20
Process 9232 exited with status = 0 (0x00000000) 
(lldb) q
```

### OSX

**Note:** JIT debugging *does not* work on OSX with the official LLVM Release 4.0. Though some breakpoint locations are resolved successfully when enabling the setting `plugin.jit-loader.gdb.enable-jit-breakpoint` (as shown below), they will still not hit.

```
osx:build user$ cmake -DCMAKE_OSX_SYSROOT=macosx10.12 -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR=/path/to/llvm40-build/lib/cmake/llvm ../JitFromScratch
osx:build user$ cmake --build .
osx:build user$ lldb -- JitFromScratch 
(lldb) target create "JitFromScratch"
Current executable set to 'JitFromScratch' (x86_64).
(lldb) log enable lldb jit
(lldb) settings set plugin.jit-loader.gdb.enable-jit-breakpoint true
(lldb) b integerDistance
Breakpoint 1: no locations (pending).
WARNING:  Unable to resolve breakpoint to any actual locations.
(lldb) r
JITLoaderGDB::SetJITBreakpoint looking for JIT register hook
JITLoaderGDB::SetJITBreakpoint setting JIT breakpoint
Process 2924 launched: '~/JitFromScratch/build/JitFromScratch' (x86_64)
JITLoaderGDB::JITDebugBreakpointHit hit JIT breakpoint
JITLoaderGDB::ReadJITDescriptorImpl registering JIT entry at 0x103d07f40 (788 bytes)
1 location added to breakpoint 1
Integer Distances: 3, 0, 3

JITLoaderGDB::JITDebugBreakpointHit hit JIT breakpoint
JITLoaderGDB::ReadJITDescriptorImpl unregistering JIT entry at 0x103d07f40
Process 2924 exited with status = 0 (0x00000000) 
(lldb) q
```

### Windows

**Note:** JIT debugging *does not* work on Windows with the official LLVM Release 5.0.

```
C:\JitFromScratch\build>cmake -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR=/path/to/llvm50-build/lib/cmake/llvm ../JitFromScratch
C:\JitFromScratch\build>cmake --build .
C:\JitFromScratch\build>lldb -- JitFromScratch.exe
(lldb) target create "JitFromScratch.exe"
Current executable set to 'JitFromScratch.exe' (x86_64).
(lldb) log enable lldb jit
(lldb) b integerDistance
Breakpoint 1: no locations (pending).
WARNING:  Unable to resolve breakpoint to any actual locations.
(lldb) r
JITLoaderGDB::JITLoaderGDB::SetJITBreakpoint looking for JIT register hook
JITLoaderGDB::JITLoaderGDB::SetJITBreakpoint looking for JIT register hook
JITLoaderGDB::JITLoaderGDB::SetJITBreakpoint looking for JIT register hook
[...]
Process 2732 launching
(lldb) Process 2732 launched: 'C:\JitFromScratch\build\JitFromScratch.exe' (x86_64)
(lldb) JITLoaderGDB::JITLoaderGDB::SetJITBreakpoint looking for JIT register hook
JITLoaderGDB::JITLoaderGDB::SetJITBreakpoint looking for JIT register hook
JITLoaderGDB::JITLoaderGDB::SetJITBreakpoint looking for JIT register hook
JITLoaderGDB::JITLoaderGDB::SetJITBreakpoint looking for JIT register hook
JITLoaderGDB::JITLoaderGDB::SetJITBreakpoint looking for JIT register hook
JITLoaderGDB::JITLoaderGDB::SetJITBreakpoint looking for JIT register hook
Process 2732 exited with status = 0 (0x00000000)
(lldb) q
```
