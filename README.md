:warning: This repository works with LLVM versions 4.0 to 8.0 and is now archived. Please find examples for more recent versions in-tree: https://github.com/llvm/llvm-project/blob/main/llvm/examples/OrcV2Examples/

<br>
<br>

# JitFromScratch

<a href="https://github.com/weliveindetail/JitFromScratch/blob/master/LICENSE.md"><img src="https://licensebuttons.net/l/by-sa/3.0/88x31.png" alt="CC BY-SA 4.0" width="57" height="20"></a> [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/master)](https://travis-ci.org/weliveindetail/JitFromScratch/branches/) <a href="https://gitpod.io/#https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a>

Collection of examples from my talks in the [LLVM Social Berlin](https://www.meetup.com/de-DE/LLVM-Social-Berlin/) and [C++ User Group Berlin](https://www.meetup.com/de-DE/berlincplusplus/) that implement various aspects of a JIT compiler based on the LLVM Orc libraries.

## Contribute

The repository follows a perfect history policy to foster traceability and understanding. If you find a bug or want to submit an improvement, please file a pull request against the [respective step](
https://github.com/weliveindetail/JitFromScratch/branches/all?query=llvm09/steps/A).

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

The example project is built in a series of self-contained steps. You can immediatedly explore and build each change in your Browser with [GitPod](https://www.gitpod.io/):

| Step | Change | Travis | GitPod | Description |
| ---- | ------ | ------ | ------ | ----------- |
| 0  | [`87cb540`](https://github.com/weliveindetail/JitFromScratch/commit/87cb540) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A00)](https://travis-ci.org/weliveindetail/JitFromScratch/branches/) | <a href="https://gitpod.io/#CHECKOUT=87cb540/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Wire up LLVM |
| 1  | [`82465fc`](https://github.com/weliveindetail/JitFromScratch/commit/82465fc) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A01)](https://travis-ci.org/weliveindetail/JitFromScratch/branches/) | <a href="https://gitpod.io/#CHECKOUT=82465fc/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Add lit tests |
| 2  | [`b1b268b`](https://github.com/weliveindetail/JitFromScratch/commit/b1b268b) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A02)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622288113) | <a href="https://gitpod.io/#CHECKOUT=b1b268b/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Add TravisCI config to run tests in prebuilt docker |
| 3  | [`81a9d8b`](https://github.com/weliveindetail/JitFromScratch/commit/81a9d8b) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A03)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622288118) | <a href="https://gitpod.io/#CHECKOUT=81a9d8b/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Initialize LLVM for native target codegen |
| 4  | [`731f482`](https://github.com/weliveindetail/JitFromScratch/commit/731f482) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A04)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622288133) | <a href="https://gitpod.io/#CHECKOUT=731f482/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Add minimal JIT compiler based on LLJIT |
| 5  | [`73a565a`](https://github.com/weliveindetail/JitFromScratch/commit/73a565a) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A05)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622288131) | <a href="https://gitpod.io/#CHECKOUT=73a565a/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Create empty module and pass it to the JIT |
| 6  | [`7690ac1`](https://github.com/weliveindetail/JitFromScratch/commit/7690ac1) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A06)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622288129) | <a href="https://gitpod.io/#CHECKOUT=7690ac1/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | In debug mode dump extra info when passing -debug -debug-only=jitfromscratch |
| 7  | [`3b47ce8`](https://github.com/weliveindetail/JitFromScratch/commit/3b47ce8) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A07)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622288139) | <a href="https://gitpod.io/#CHECKOUT=3b47ce8/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Generate function that takes two ints and returns zero |
| 8  | [`8a4bba8`](https://github.com/weliveindetail/JitFromScratch/commit/8a4bba8) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A08)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622288135) | <a href="https://gitpod.io/#CHECKOUT=8a4bba8/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Add basic sanity checks for IR code |
| 9  | [`402e9d1`](https://github.com/weliveindetail/JitFromScratch/commit/402e9d1) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A09)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622288137) | <a href="https://gitpod.io/#CHECKOUT=402e9d1/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Request and run trivial function |
| 10 | [`e08ff57`](https://github.com/weliveindetail/JitFromScratch/commit/e08ff57) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A10)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622288108) | <a href="https://gitpod.io/#CHECKOUT=e08ff57/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Emit IR code for substraction and use it in the integerDistances function |
| 11 | [`8cf690b`](https://github.com/weliveindetail/JitFromScratch/commit/8cf690b) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A11)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622289671) | <a href="https://gitpod.io/#CHECKOUT=8cf690b/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Emit variable names to make IR more readable |
| 12 | [`03b8701`](https://github.com/weliveindetail/JitFromScratch/commit/03b8701) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A12)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622289679) | <a href="https://gitpod.io/#CHECKOUT=03b8701/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Allow symbol resolution from the host process |
| 13 | [`a0cdaad`](https://github.com/weliveindetail/JitFromScratch/commit/a0cdaad) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A13)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622289663) | <a href="https://gitpod.io/#CHECKOUT=a0cdaad/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Emit call to stdlib function abs in JITed code |
| 14 | [`2653289`](https://github.com/weliveindetail/JitFromScratch/commit/2653289) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A14)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622289675) | <a href="https://gitpod.io/#CHECKOUT=2653289/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Emit unrolled loop in JITed code |
| 15 | [`2291ee4`](https://github.com/weliveindetail/JitFromScratch/commit/2291ee4) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A15)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622289677) | <a href="https://gitpod.io/#CHECKOUT=2291ee4/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Emit call to allocator in JITed code |
| 16 | [`bf72028`](https://github.com/weliveindetail/JitFromScratch/commit/bf72028) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A16)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622289656) | <a href="https://gitpod.io/#CHECKOUT=bf72028/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Remove wrapper function and call JITed code directly |
| 17 | [`96f4c8e`](https://github.com/weliveindetail/JitFromScratch/commit/96f4c8e) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A17)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622289669) | <a href="https://gitpod.io/#CHECKOUT=96f4c8e/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Break free from LLJIT |
| 18 | [`a4b88a0`](https://github.com/weliveindetail/JitFromScratch/commit/a4b88a0) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A18)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622289661) | <a href="https://gitpod.io/#CHECKOUT=a4b88a0/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Implement GDB JIT interface |
| 19 | [`b7212c3`](https://github.com/weliveindetail/JitFromScratch/commit/b7212c3) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A19)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622289659) | <a href="https://gitpod.io/#CHECKOUT=b7212c3/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Add optimization passes controlled via -Ox command line flag |
| 20 | [`75dff62`](https://github.com/weliveindetail/JitFromScratch/commit/75dff62) | [![Build Status](https://travis-ci.org/weliveindetail/JitFromScratch.svg?branch=llvm09/steps/A20)](https://travis-ci.org/weliveindetail/JitFromScratch/builds/622289673) | <a href="https://gitpod.io/#CHECKOUT=75dff62/https://github.com/weliveindetail/JitFromScratch"><img src="https://gitpod.io/button/open-in-gitpod.svg" alt="Open in Gitpod" width="86" height="20"></a> | Simple name-based object cache |

## Build and run locally

```
$ git clone https://github.com/weliveindetail/JitFromScratch jitfromscratch
$ mkdir build && cd build
$ cmake -GNinja -DLLVM_DIR=/path/to/llvm-build/lib/cmake/llvm ../jitfromscratch
$ ninja run
```

The project was tested on Ubuntu 19.04 and macOS 10.14. Please find real-world examples for commands and expected output on [Travis CI](https://travis-ci.org/weliveindetail/JitFromScratch).

## Build and run in docker
```
$ git clone https://github.com/weliveindetail/JitFromScratch jitfromscratch
$ cd jitfromscratch
$ docker run -it -v $(pwd):/host/jitfromscratch -e REMOTE=/host/jitfromscratch -e CHECKOUT=$(git rev-parse HEAD) weliveindetail/jitfromscratch:llvm09-test-release
```

Further images can be pulled from [dockerhub](https://cloud.docker.com/u/weliveindetail/repository/docker/weliveindetail/jitfromscratch) or created manually based on the [given Dockerfiles](https://github.com/weliveindetail/JitFromScratch/tree/master/docs/docker).

## Setup vscode

Install the [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), [
CMake Tools](https://marketplace.visualstudio.com/items?itemName=vector-of-bool.cmake-tools) and [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb) extensions and use the [given configuration files](https://github.com/weliveindetail/JitFromScratch/tree/master/docs/vscode).

## Previous Versions

You can easily diff for a specific 8.0 to 9.0 change set, e.g.:
```
$ git diff origin/llvm08/master origin/llvm09/master
$ git diff origin/llvm08/steps/A00 origin/llvm09/steps/A00 -- CMakeLists.txt
$ git diff origin/llvm08/steps/A18 origin/llvm09/steps/A18 -- JitFromScratch.cpp
```

In LLVM 8, the Orc library was almost entirely rewritten and I used the opportunity to also refine my examples. You can still find the old examples here:

* [LLVM 8.0](https://github.com/weliveindetail/JitFromScratch/tree/master/versions/llvm08)
* [LLVM 5.0](https://github.com/weliveindetail/JitFromScratch/tree/master/versions/llvm05)
* [LLVM 4.0](https://github.com/weliveindetail/JitFromScratch/tree/master/versions/llvm04)
