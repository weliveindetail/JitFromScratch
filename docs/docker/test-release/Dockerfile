# JitFromScratch docker image for running lit tests
#
# The image ships a prebuilt release version of LLVM 9 and a checkout of the
# JitFromScratch sources. By default the container will run the tests on a
# build from the llvm09/master branch. Passing values for the environment
# variables REMOTE and CHECKOUT allows to test arbitrary forks and/or revisions.
#
#   $ docker build -t <image-name> /path/to/JitFromScratch/docs/docker/test-release
#   $ docker run -e REMOTE=<repo> -e CHECKOUT=<commit> <image-name>
#
FROM ubuntu:19.04
LABEL maintainer="weliveindetail <stefan.graenitz@gmail.com>"

# Install tools and libs for building LLVM and JitFromScratch
RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates build-essential git cmake cmake-data \
        ninja-build clang lld zlib1g-dev python3-dev && \
    rm -rf /var/lib/apt/lists/*

# Build and install partial LLVM distribution
RUN git clone --depth=1 --branch=release/9.x \
        https://github.com/llvm/llvm-project.git /home/llvm-project && \
    \
    # Configure
    CC=clang CXX=clang++ cmake \
        -G Ninja \
        -S /home/llvm-project/llvm \
        -B /home/llvm-build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local/opt/llvm \
        -DLLVM_TARGETS_TO_BUILD=host \
        -DLLVM_INCLUDE_TESTS=OFF \
        -DLLVM_USE_LINKER=lld \
        -DLLVM_INSTALL_UTILS=ON \
        -DLLVM_DISTRIBUTION_COMPONENTS="cmake-exports;llvm-headers;FileCheck;LLVMAggressiveInstCombine;LLVMAnalysis;LLVMAsmParser;LLVMAsmPrinter;LLVMBinaryFormat;LLVMBitReader;LLVMBitWriter;LLVMBitstreamReader;LLVMCodeGen;LLVMCore;LLVMDebugInfoCodeView;LLVMDebugInfoDWARF;LLVMDebugInfoMSF;LLVMDemangle;LLVMExecutionEngine;LLVMGlobalISel;LLVMIRReader;LLVMInstCombine;LLVMInstrumentation;LLVMJITLink;LLVMLinker;LLVMMC;LLVMMCDisassembler;LLVMMCParser;LLVMObject;LLVMOrcJIT;LLVMProfileData;LLVMRemarks;LLVMRuntimeDyld;LLVMScalarOpts;LLVMSelectionDAG;LLVMSupport;LLVMTarget;LLVMTransformUtils;LLVMVectorize;LLVMX86AsmParser;LLVMX86CodeGen;LLVMX86Desc;LLVMX86Info;LLVMX86Utils;LLVMipo" && \
    \
    # Build and install
    ninja -C /home/llvm-build install-distribution && \
    \
    # There is no install target for the LIT driver at the moment
    cp /home/llvm-build/bin/llvm-lit /usr/local/opt/llvm/bin/llvm-lit && \
    cp -R /home/llvm-project/llvm/utils/lit/lit /usr/local/opt/llvm/bin/lit && \
    \
    # Remove build and source-tree to reduce image size
    rm -rf /home/llvm-project && \
    rm -rf /home/llvm-build

# Checkout JitFromScratch
RUN git clone https://github.com/weliveindetail/JitFromScratch.git \
              /home/jitfromscratch && \
    cd /home/jitfromscratch && \
    git checkout llvm09/master

# By default: update, build, run and test on startup
CMD set -x && \
    cd /home/jitfromscratch && \
    ([ ! "${REMOTE+1}" ] || git remote set-url origin "${REMOTE}") && \
    git fetch --quiet origin && \
    git checkout --quiet FETCH_HEAD && \
    ([ ! "${CHECKOUT+1}" ] || git checkout --quiet "${CHECKOUT}") && \
    git log -1 -s && \
    CC=clang CXX=clang++ cmake \
      -G Ninja \
      -S /home/jitfromscratch \
      -B /home/jitfromscratch-release \
      -DCMAKE_BUILD_TYPE=Release && \
    ninja -C /home/jitfromscratch-release -v run && \
    ninja -C /home/jitfromscratch-release -v check
