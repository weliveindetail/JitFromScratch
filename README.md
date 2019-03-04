# JitFromScratch

[![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A14)](https://travis-ci.org/weliveindetail/JitFromScratch/branches/)

Collection of examples from my talks in the [LLVM Social Berlin](https://www.meetup.com/de-DE/LLVM-Social-Berlin/) and [C++ User Group Berlin](https://www.meetup.com/de-DE/berlincplusplus/) that implement various aspects of a JIT compiler based on the LLVM Orc libraries.

Please find the full readme and documentation on [the master branch](https://github.com/weliveindetail/JitFromScratch).

## Contribute

The repository follows a perfect history policy to foster traceability and understanding. If you find a bug or want to submit an improvement, please file a pull request against the [respective step](
https://github.com/weliveindetail/JitFromScratch/branches/all?query=step).

## Build and run locally

```
$ git clone https://github.com/weliveindetail/JitFromScratch jitfromscratch
$ mkdir build && cd build
$ cmake -GNinja -DLLVM_DIR=/path/to/llvm-build/lib/cmake/llvm ../jitfromscratch
$ ninja run
```
