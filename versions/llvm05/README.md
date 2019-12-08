# JitFromScratch
Collection of examples from my talks in the [LLVM Social Berlin](https://www.meetup.com/de-DE/LLVM-Social-Berlin/) and [C++ User Group Berlin](https://www.meetup.com/de-DE/berlincplusplus/) that showcase techniques for implementing various aspects of a JIT compiler built with the LLVM 5.0 ORC libraries. The repository follows a *perfect history* policy to foster traceability and understanding.

**This is the LLVM 5.0 compatible version of the examples. Please find latest updates [here](https://github.com/weliveindetail/JitFromScratch).**

## Build

All examples are tested on Linux Mint 18, Mac OS X 10.12 and Windows 10. Please find detailed instructions in [Building a JIT from scratch](https://weliveindetail.github.io/blog/post/2017/07/18/building-a-jit-from-scratch.html).

## Structure

The [jit-basics](https://github.com/weliveindetail/JitFromScratch/commits/llvm50/jit-basics) establish a common code base for all further examples by demonstrating how to compile the code for a simple function at runtime:

```
template <size_t sizeOfArray>
int *integerDistances(const int (&x)[sizeOfArray], int *y) {
  int items = arrayElements(x);
  int *results = customIntAllocator(items);

  for (int i = 0; i < items; i++) {
    results[i] = abs(x[i] - y[i]);
  }

  return results;
}
```

## Table of Contents

* general topics
  * [jit-basics](https://github.com/weliveindetail/JitFromScratch/commits/llvm50/jit-basics) — step by step guide for a basic minimal JIT [[readme](https://github.com/weliveindetail/JitFromScratch/tree/llvm50/jit-basics)]
  * [jit-optimization](https://github.com/weliveindetail/JitFromScratch/commits/llvm50/jit-optimization) — applying selected optimizations to JITed code [[readme](https://github.com/weliveindetail/JitFromScratch/tree/llvm50/jit-optimization)]

* calling external functions from JITed code
  * [jit-function-calls/explicit](https://github.com/weliveindetail/JitFromScratch/commits/llvm50/jit-function-calls/explicit) — explicit run-time name/address mapping [[readme](https://github.com/weliveindetail/JitFromScratch/tree/llvm50/jit-function-calls/explicit)]
  * [jit-function-calls/implicit-extern-c](https://github.com/weliveindetail/JitFromScratch/commits/llvm50/jit-function-calls/implicit-extern-c) — implicit compile-time binding to C functions [[readme](https://github.com/weliveindetail/JitFromScratch/tree/llvm50/jit-function-calls/implicit-extern-c)]
  * [jit-function-calls/implicit-mangled](https://github.com/weliveindetail/JitFromScratch/commits/llvm50/jit-function-calls/implicit-mangled) — implicit compile-time binding to C++ functions [[readme](https://github.com/weliveindetail/JitFromScratch/tree/llvm50/jit-function-calls/implicit-mangled)]

* debugging JIT compiled code
  * [jit-debug/llvm-debug-dumps](https://github.com/weliveindetail/JitFromScratch/commits/llvm50/jit-debug/llvm-debug-dumps) — command line arguments `-debug` and `-debug-only` [[readme](https://github.com/weliveindetail/JitFromScratch/tree/llvm50/jit-debug/llvm-debug-dumps)]
  * [jit-debug/gdb-interface](https://github.com/weliveindetail/JitFromScratch/commits/llvm50/jit-debug/gdb-interface) — implement the GDB JIT-interface [[readme](https://github.com/weliveindetail/JitFromScratch/tree/llvm50/jit-debug/gdb-interface)]

* compile source code at runtime
  * [jit-from-source/cpp-clang](https://github.com/weliveindetail/JitFromScratch/commits/llvm50/jit-from-source/cpp-clang) — compile C++ with Clang at runtime [[readme](https://github.com/weliveindetail/JitFromScratch/tree/llvm50/jit-from-source/cpp-clang)]

## Previous Versions

* [LLVM 4.0](https://github.com/weliveindetail/JitFromScratch/tree/master/llvm40)

You can easily diff for a specific 4.0 to 5.0 change set, e.g.:
```
$ git diff llvm40/jit-basics llvm50/jit-basics
$ git diff llvm40/jit-basics llvm50/jit-basics -- CMakeLists.txt
$ git diff llvm40/jit-debug/gdb-interface llvm50/jit-debug/gdb-interface -- SimpleOrcJit.h
```

## Related Blog Posts

* [LLVM 5.0 Release ORC API Changes](http://weliveindetail.github.io/blog/post/2017/08/23/llvm50-release-orc-api-changes.html)
* [The simplest way to compile C++ with Clang at runtime](http://weliveindetail.github.io/blog/post/2017/07/25/compile-with-clang-at-runtime-simple.html)
* [Debugging Clang](http://weliveindetail.github.io/blog/post/2017/07/19/debugging-clang.html)
* [Building a JIT from scratch](http://weliveindetail.github.io/blog/post/2017/07/18/building-a-jit-from-scratch.html)
* [Notes on setting up projects with LLVM](http://weliveindetail.github.io/blog/post/2017/07/17/notes-setup.html)
