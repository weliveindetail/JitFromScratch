# JitFromScratch docker image for running lit tests
#
# The image ships a prebuilt release version of LLVM 8 and a checkout of the
# JitFromScratch sources. By default the container will run the tests on a
# build from the llvm80/jit-basics branch. Passing values for the environment
# variables REMOTE and CHECKOUT allows to test arbitrary forks and/or revisions.
#
#   $ docker build -t <image-name> /path/to/JitFromScratch/docker/test-release
#   $ docker run -e REMOTE=<repo> -e CHECKOUT=<commit> <image-name>
#
FROM ubuntu:18.04
LABEL maintainer "weliveindetail <stefan.graenitz@gmail.com>"

# Install tools and libs for building LLVM and JitFromScratch
RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates build-essential git cmake cmake-data \
        ninja-build clang zlib1g-dev python3.6

# Checkout and configure LLVM
RUN git clone --depth=1 --branch=release/8.x https://github.com/llvm/llvm-project.git llvm-release80
RUN mkdir llvm-release80-release && \
    cd llvm-release80-release && \
    CC=clang CXX=clang++ \
        cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD=host ../llvm-release80/llvm

# Distribute libraries accross separate layers to limit the number of lost
# artifacts if docker-build runs out of memory. The list is obtained from:
# llvm-config --libnames core executionengine native orcjit runtimedyld support
RUN cd llvm-release80-release && ninja llvm-config FileCheck
RUN cd llvm-release80-release && ninja LLVMSupport LLVMDemangle LLVMCore LLVMBinaryFormat
RUN cd llvm-release80-release && ninja LLVMRuntimeDyld LLVMObject LLVMMCParser LLVMBitReader LLVMMC LLVMDebugInfoCodeView LLVMDebugInfoMSF
RUN cd llvm-release80-release && ninja LLVMExecutionEngine LLVMTarget LLVMAnalysis LLVMProfileData LLVMOrcJIT LLVMTransformUtils
RUN cd llvm-release80-release && ninja LLVMGlobalISel LLVMSelectionDAG LLVMScalarOpts LLVMInstCombine LLVMAggressiveInstCombine
RUN cd llvm-release80-release && ninja LLVMAsmPrinter LLVMCodeGen LLVMMCDisassembler LLVMBitWriter LLVMBinaryFormat
RUN cd llvm-release80-release && ninja LLVMX86AsmPrinter LLVMX86CodeGen LLVMX86Disassembler LLVMX86AsmParser LLVMX86Desc LLVMX86Info LLVMX86Utils
RUN cd llvm-release80-release && ninja LLVMipo LLVMInstrumentation LLVMVectorize LLVMLinker LLVMIRReader LLVMAsmParser

# Checkout JitFromScratch
RUN git clone https://github.com/weliveindetail/JitFromScratch.git jitfromscratch && \
    cd jitfromscratch && \
    git checkout llvm80/jit-basics && \
    mkdir ../build

# By default: update, build and run tests on startup
CMD set -x && \
    cd jitfromscratch && \
    ([ ! "${REMOTE+1}" ] || git remote set-url origin "${REMOTE}") && \
    git fetch --quiet origin && \
    git checkout --quiet FETCH_HEAD && \
    ([ ! "${CHECKOUT+1}" ] || git checkout --quiet "${CHECKOUT}") && \
    git log -1 -s && \
    cd ../build && \
    CC=clang CXX=clang++ \
        cmake -GNinja -DLLVM_DIR=/llvm-release80-release/lib/cmake/llvm ../jitfromscratch && \
    ninja run && \
    ninja check
