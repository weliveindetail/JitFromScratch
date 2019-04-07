# JitFromScratch

[![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm80/steps/A21)](https://travis-ci.org/weliveindetail/JitFromScratch/branches/)

Collection of examples from my talks in the [LLVM Social Berlin](https://www.meetup.com/de-DE/LLVM-Social-Berlin/) and [C++ User Group Berlin](https://www.meetup.com/de-DE/berlincplusplus/) that implement various aspects of a JIT compiler based on the LLVM Orc libraries.

## Contribute

The repository follows a perfect history policy to foster traceability and understanding. If you find a bug or want to submit an improvement, please send a pull request against the [respective step](
https://github.com/weliveindetail/JitFromScratch/branches/all?query=step).

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

Docker images with prebuilt variants of LLVM can be pulled from [dockerhub](https://cloud.docker.com/u/weliveindetail/repository/docker/weliveindetail/jitfromscratch) or created manually based on the Dockerfiles in [docker/](https://github.com/weliveindetail/JitFromScratch/tree/master/docker).

## Build and debug from vscode

Install the [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), [
CMake Tools](https://marketplace.visualstudio.com/items?itemName=vector-of-bool.cmake-tools) and [Native Debug](https://marketplace.visualstudio.com/items?itemName=webfreak.debug) extensions and apply the configuration files in [docs/vscode/](https://github.com/weliveindetail/JitFromScratch/tree/master/docs/vscode).

## Previous Versions

I try to keep modifications on the examples minimal when porting to a newer version of LLVM. This allows to [diff for API changes](https://github.com/weliveindetail/JitFromScratch/tree/master/llvm50#previous-versions) and see how they affect individual steps.

In LLVM 8, however, the Orc library was almost entirely rewritten and I used the opportunity to also refine my examples.

You can still find the old examples here:

* [LLVM 5.0](https://github.com/weliveindetail/JitFromScratch/tree/master/llvm50)
* [LLVM 4.0](https://github.com/weliveindetail/JitFromScratch/tree/master/llvm40)
