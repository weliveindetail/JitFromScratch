# JitFromScratch / llvm40 / jit-from-source / cpp-clang

This branch shows a simple way to compile C++ code at runtime using the Clang cc1 frontend tool. Clang is linked into the executable statically and invoked via `cc1_main`. Please find a detailed description in my blog post [The simplest way to compile C++ with Clang at runtime](http://weliveindetail.github.io/blog/post/2017/07/25/compile-with-clang-at-runtime-simple.html).

The example uses almost the same code as was the basis for the [`jit-basics`](https://github.com/weliveindetail/JitFromScratch/tree/llvm40/jit-basics) branch:

```
// Implementation of the integerDistances function.
std::string sourceCode =
    "extern \"C\" int abs(int);                                           \n"
    "extern int *customIntAllocator(unsigned items);                      \n"
    "                                                                     \n"
    "extern \"C\" int *integerDistances(int* x, int *y, unsigned items) { \n"
    "  int *results = customIntAllocator(items);                          \n"
    "                                                                     \n"
    "  for (int i = 0; i < items; i++) {                                  \n"
    "    results[i] = abs(x[i] - y[i]);                                   \n"
    "  }                                                                  \n"
    "                                                                     \n"
    "  return results;                                                    \n"
    "}                                                                    \n";
```

## Build and Run

Tested on Linux Mint 18, Mac OS X 10.12 and Windows 10. For detailed instructions please refer to [Building a JIT from scratch](https://weliveindetail.github.io/blog/post/2017/07/18/building-a-jit-from-scratch.html).

```
$ cmake -DLLVM_DIR=/path/to/build-llvm40/lib/cmake/llvm -DCMAKE_BUILD_TYPE=Debug ../JitFromScratch
$ cmake --build .
$ ./JitFromScratch -debug -debug-only=cc1driver
Invoke Clang cc1 with args:
-emit-llvm -emit-llvm-bc -emit-llvm-uselists -main-file-name 
/var/folders/n7/wxcpxmvd1yx5pld9wr8tp1dm0000gn/T/JitFromScratch-90fb6c.cpp
-std=c++14 -disable-free -stack-protector 1 -fmax-type-align=16 -mrelocation-model 
pic -pic-level 2 -mthread-model posix -mdisable-fp-elim -masm-verbose -munwind-tables 
-dwarf-column-info -debugger-tuning=lldb -debug-info-kind=standalone -dwarf-version=4 
-stdlib=libc++ -resource-dir ~/Develop/llvm40/build-llvm40-clang/lib/clang/4.0.1
-o /var/folders/n7/wxcpxmvd1yx5pld9wr8tp1dm0000gn/T/JitFromScratch-90fb6c.bc 
-x c++ /var/folders/n7/wxcpxmvd1yx5pld9wr8tp1dm0000gn/T/JitFromScratch-90fb6c.cpp 

Integer Distances: 3, 0, 3
```
