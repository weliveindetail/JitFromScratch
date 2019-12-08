# JitFromScratch docker image for remote debugging
#
# The image ships a checkout of the JitFromScratch sources and the required
# components from LLVM 9 prebuilt in debug mode. On startup the container builds
# JitFromScratch from the latest llvm90/master branch and runs lldb-server in
# platform mode to allow remote debugging. Passing values for the environment
# variables REMOTE and CHECKOUT allows to use arbitrary forks and/or revisions.
#
# Run remote host:
#   $ docker build -t <image-name> /path/to/jitfromscratch/docs/docker/debug
#   $ docker run --privileged --cap-add=SYS_PTRACE \
#                --security-opt seccomp=unconfined \
#                --security-opt apparmor=unconfined \
#                -p 9000-9001:9000-9001 -e REMOTE=<repo> -e CHECKOUT=<commit> <image-name>
#
# Debug from local host:
#   $ lldb
#   (lldb) platform select remote-linux
#   (lldb) platform connect connect://localhost:9000
#   (lldb) target create /workspace/jitfromscratch-debug/JitFromScratch
#   (lldb) settings set target.source-map \
#            /workspace/jitfromscratch /path/to/jitfromscratch \
#            /workspace/llvm-project /path/to/jitfromscratch/build/llvm-project
#   (lldb) b main
#   Breakpoint 1: where = JitFromScratch`main + 26 at main.cpp:135, address = 0x0000000000b0ffaa
#   (lldb) process launch
#      ...
#      134  int main(int argc, char **argv) {
#   -> 135    InitLLVM X(argc, argv);
#      ...
#
FROM ubuntu:19.04
LABEL maintainer="weliveindetail <stefan.graenitz@gmail.com>"

# Install tools and libs for building LLVM and JitFromScratch
RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates build-essential git cmake cmake-data \
        ninja-build clang lld lldb zlib1g-dev python3-dev && \
    rm -rf /var/lib/apt/lists/*

# Build the parts we need from LLVM. Keep source- and build-tree around, so we
# find the source files when debugging without extra effort.
RUN git clone --depth=1 --branch=release/9.x \
        https://github.com/llvm/llvm-project.git /workspace/llvm-project && \
    \
    # Configure
    CC=clang CXX=clang++ cmake \
        -G Ninja \
        -S /workspace/llvm-project/llvm \
        -B /workspace/llvm-build \
        -DCMAKE_BUILD_TYPE=Debug \
        -DLLVM_TARGETS_TO_BUILD=host \
        -DLLVM_INCLUDE_TESTS=OFF \
        -DLLVM_USE_LINKER=lld && \
    \
    # Build
    ninja -C /workspace/llvm-build \
      FileCheck LLVMAggressiveInstCombine LLVMAnalysis LLVMAsmParser \
      LLVMAsmPrinter LLVMBinaryFormat LLVMBitReader LLVMBitWriter \
      LLVMBitstreamReader LLVMCodeGen LLVMCore LLVMDebugInfoCodeView \
      LLVMDebugInfoDWARF LLVMDebugInfoMSF LLVMDemangle LLVMExecutionEngine \
      LLVMGlobalISel LLVMIRReader LLVMInstCombine LLVMInstrumentation \
      LLVMJITLink LLVMLinker LLVMMC LLVMMCDisassembler LLVMMCParser LLVMObject \
      LLVMOrcJIT LLVMPasses LLVMProfileData LLVMRemarks LLVMRuntimeDyld LLVMScalarOpts \
      LLVMSelectionDAG LLVMSupport LLVMTarget LLVMTransformUtils LLVMVectorize \
      LLVMX86AsmParser LLVMX86CodeGen LLVMX86Desc LLVMX86Info LLVMX86Utils \
      LLVMipo

# Checkout JitFromScratch
RUN git clone https://github.com/weliveindetail/JitFromScratch.git \
              /workspace/jitfromscratch && \
    cd /workspace/jitfromscratch && \
    git checkout llvm09/master

# By default: update, build and run JitFromScratch in gdbserver on startup
CMD set -x && \
    cd /workspace/jitfromscratch && \
    ([ ! "${REMOTE+1}" ] || git remote set-url origin "${REMOTE}") && \
    git fetch --quiet origin && \
    git checkout --quiet FETCH_HEAD && \
    ([ ! "${CHECKOUT+1}" ] || git checkout --quiet "${CHECKOUT}") && \
    git log -1 -s && \
    CC=clang CXX=clang++ cmake \
      -G Ninja \
      -S /workspace/jitfromscratch \
      -B /workspace/jitfromscratch-debug \
      -DCMAKE_BUILD_TYPE=Debug \
      -DLLVM_DIR=/workspace/llvm-build/lib/cmake/llvm && \
    ninja -C /workspace/jitfromscratch-debug JitFromScratch && \
    lldb-server platform --server --listen 0.0.0.0:9000 --gdbserver-port 9001
