# JitFromScratch

<a href="https://github.com/weliveindetail/JitFromScratch/blob/master/LICENSE.md"><img src="https://licensebuttons.net/l/by-sa/3.0/88x31.png" alt="CC BY-SA 4.0" width="57" height="20"></a> [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=master)](https://travis-ci.org/weliveindetail/JitFromScratch/branches/)

Collection of examples from my talks in the [LLVM Social Berlin](https://www.meetup.com/de-DE/LLVM-Social-Berlin/) and [C++ User Group Berlin](https://www.meetup.com/de-DE/berlincplusplus/) that implement various aspects of a JIT compiler based on the LLVM Orc libraries.

## Contribute

The repository follows a perfect history policy to foster traceability and understanding. If you find a bug or want to submit an improvement, please send a pull request against the [respective step](
https://github.com/weliveindetail/JitFromScratch/branches/all?query=llvm08/steps/A).

## Structure

The examples are organized around a nonsense command line program that compiles the code for a simple function at runtime:

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

## Steps

The example project is built in a series of self-contained steps:

0. [Add sources with original integerDistances function](https://github.com/weliveindetail/JitFromScratch/commit/1e459e7)
1. [Add lit tests](https://github.com/weliveindetail/JitFromScratch/commit/822acce)
2. [Add Dockerfiles to run typical use-cases on Ubuntu 18.04](https://github.com/weliveindetail/JitFromScratch/commit/bafa17e)
3. [Add TravisCI config for test-debug docker](https://github.com/weliveindetail/JitFromScratch/commit/fb5ea32)
4. [Initialize LLVM for native target codegen](https://github.com/weliveindetail/JitFromScratch/commit/9bf5f20)
5. [Add minimal JIT compiler based on LLJIT](https://github.com/weliveindetail/JitFromScratch/commit/58e6fea)
6. [Create empty module and pass it to the JIT](https://github.com/weliveindetail/JitFromScratch/commit/0143437)
7. [In debug mode dump extra info when passing -debug -debug-only=jitfromscratch](https://github.com/weliveindetail/JitFromScratch/commit/ec22b9a)
8. [Generate function that takes two ints and returns zero](https://github.com/weliveindetail/JitFromScratch/commit/3056f11)
9. [Add basic sanity checks for IR code](https://github.com/weliveindetail/JitFromScratch/commit/a8229d0)
10. [Request and run trivial function](https://github.com/weliveindetail/JitFromScratch/commit/afb42bd)
11. [Emit IR code for substraction and use it in the integerDistances function](https://github.com/weliveindetail/JitFromScratch/commit/5a2ff20)
12. [Emit variable names to make IR more readable](https://github.com/weliveindetail/JitFromScratch/commit/8bfbbef)
13. [Allow symbol resolution from the host process](https://github.com/weliveindetail/JitFromScratch/commit/78d8753)
14. [Emit call to stdlib function abs in JITed code](https://github.com/weliveindetail/JitFromScratch/commit/54964d7)
15. [Emit unrolled loop in JITed code](https://github.com/weliveindetail/JitFromScratch/commit/79c608f)
16. [Emit call to allocator in JITed code](https://github.com/weliveindetail/JitFromScratch/commit/a946c44)
17. [Remove wrapper function and call JITed code directly](https://github.com/weliveindetail/JitFromScratch/commit/8b8fcdd)
18. [Break free from LLJIT](https://github.com/weliveindetail/JitFromScratch/commit/98406a6)
19. [Implement GDB JIT interface](https://github.com/weliveindetail/JitFromScratch/commit/4e858ef)
20. [Add optimization passes controlled via -Ox command line flag](https://github.com/weliveindetail/JitFromScratch/commit/f4c7fb5)
21. [Simple name-based object cache](https://github.com/weliveindetail/JitFromScratch/commit/88175b1)
22. [Query cache upfront](https://github.com/weliveindetail/JitFromScratch/commit/95431ab)

## Build and run locally

```
$ git clone https://github.com/weliveindetail/JitFromScratch jitfromscratch
$ mkdir build && cd build
$ cmake -GNinja -DLLVM_DIR=/path/to/llvm-build/lib/cmake/llvm ../jitfromscratch
$ ninja JitFromScratch
$ ./JitFromScratch -debug -debug-only=jitfromscratch
```
The project was tested on Ubuntu 18.04, macOS 10.14 and Windows 10. Please find real-world examples for commands and expected output on [Travis CI](https://travis-ci.org/weliveindetail/JitFromScratch).

## Build and run in docker

Docker images with prebuilt variants of LLVM can be pulled from [dockerhub](https://cloud.docker.com/u/weliveindetail/repository/docker/weliveindetail/jitfromscratch) or created manually based on the [given Dockerfiles](https://github.com/weliveindetail/JitFromScratch/tree/master/docs/docker).

## Build and debug from vscode

Install the [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), [
CMake Tools](https://marketplace.visualstudio.com/items?itemName=vector-of-bool.cmake-tools) and [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb) extensions and use the [given configuration files](https://github.com/weliveindetail/JitFromScratch/tree/master/docs/vscode).

## Previous Versions

I try to keep modifications on the examples minimal when porting to a newer version of LLVM. This allows to [diff for API changes](https://github.com/weliveindetail/JitFromScratch/tree/master/versions/llvm50#previous-versions) and see how they affect individual steps.

In LLVM 8, however, the Orc library was almost entirely rewritten and I used the opportunity to also refine my examples.

You can still find the old examples here:

* [LLVM 5.0](https://github.com/weliveindetail/JitFromScratch/tree/master/versions/llvm50)
* [LLVM 4.0](https://github.com/weliveindetail/JitFromScratch/tree/master/versions/llvm40)
