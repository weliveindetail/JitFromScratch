# JitFromScratch docker image for development in GitPod
FROM ubuntu:19.04
LABEL maintainer="weliveindetail <stefan.graenitz@gmail.com>"

# Install tools and libs for building and debugging
RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates build-essential git cmake cmake-data \
        ninja-build clang lldb lld zlib1g-dev python3-dev && \
    rm -rf /var/lib/apt/lists/*

# Build and install partial LLVM distribution
RUN git clone --depth=1 --branch=release/9.x \
        https://github.com/llvm/llvm-project.git /workspace/llvm-project && \
    \
    # Configure
    CC=clang CXX=clang++ cmake \
        -G Ninja \
        -S /workspace/llvm-project/llvm \
        -B /workspace/llvm-build \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_INSTALL_PREFIX=/usr/local/opt/llvm \
        -DLLVM_TARGETS_TO_BUILD=host \
        -DLLVM_INCLUDE_TESTS=OFF \
        -DLLVM_USE_LINKER=lld \
        -DLLVM_INSTALL_UTILS=ON \
        -DLLVM_DISTRIBUTION_COMPONENTS="cmake-exports;llvm-headers;FileCheck;LLVMAggressiveInstCombine;LLVMAnalysis;LLVMAsmParser;LLVMAsmPrinter;LLVMBinaryFormat;LLVMBitReader;LLVMBitWriter;LLVMBitstreamReader;LLVMCodeGen;LLVMCore;LLVMDebugInfoCodeView;LLVMDebugInfoDWARF;LLVMDebugInfoMSF;LLVMDemangle;LLVMExecutionEngine;LLVMGlobalISel;LLVMIRReader;LLVMInstCombine;LLVMInstrumentation;LLVMJITLink;LLVMLinker;LLVMMC;LLVMMCDisassembler;LLVMMCParser;LLVMObject;LLVMOrcJIT;LLVMPasses;LLVMProfileData;LLVMRemarks;LLVMRuntimeDyld;LLVMScalarOpts;LLVMSelectionDAG;LLVMSupport;LLVMTarget;LLVMTransformUtils;LLVMVectorize;LLVMX86AsmParser;LLVMX86CodeGen;LLVMX86Desc;LLVMX86Info;LLVMX86Utils;LLVMipo" && \
    \
    # Build and install
    ninja -C /workspace/llvm-build install-distribution && \
    \
    # There is no install target for the LIT driver at the moment
    cp /workspace/llvm-build/bin/llvm-lit /usr/local/opt/llvm/bin/llvm-lit && \
    cp -R /workspace/llvm-project/llvm/utils/lit/lit /usr/local/opt/llvm/bin/lit && \
    \
    # Keep source tree for debugging
    #rm -rf /workspace/llvm-project && \
    # Remove build-tree to reduce image size
    rm -rf /workspace/llvm-build

# Config for development gets copied in place by init command
ADD .vscode /home/gitpod/.vscode

# CMD is ignored and instead the init command from the top-level .gitpod.yml
# runs on first startup of the containter in your GitPod workspace.
