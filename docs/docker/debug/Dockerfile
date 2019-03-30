# JitFromScratch docker image for remote debugging
#
# The image ships a prebuilt debug version of LLVM 8 and a checkout of the
# JitFromScratch sources. By default the container will run gdbserver on a
# build from the llvm80/jit-basics branch. Passing values for the environment
# variables REMOTE and CHECKOUT allows to test arbitrary forks and/or revisions.
#
# Run remote host:
#   $ docker build -t <image-name> /path/to/JitFromScratch/docker/debug
#   $ docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined --security-opt apparmor=unconfined \
#                -p 9999:9999 -e REMOTE=<repo> -e CHECKOUT=<commit> <image-name>
#
# Debug from local host:
#   $ gdb -q
#   $ (gdb) directory /path/to/JitFromScratch-src
#   $ (gdb) directory /path/to/llvm-src/..
#   $ (gdb) target remote :9999
#   $ (gdb) b main
#   $ (gdb) c
#   ...
#   Breakpoint 1, main (argc=1, argv=0x7fffffffe788) at /JitFromScratch/main.cpp:109
#   109	int main(int argc, char **argv) {
#   ...
#   $ (gdb) monitor exit
#
FROM ubuntu:18.04
LABEL maintainer "weliveindetail <stefan.graenitz@gmail.com>"

# Install tools and libs for building LLVM and JitFromScratch
RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates build-essential git cmake cmake-data \
        ninja-build clang zlib1g-dev python3.6 gdbserver

# Checkout and configure LLVM
RUN git clone --depth=1 --branch=release/8.x https://github.com/llvm/llvm-project.git llvm-release80
RUN mkdir llvm-release80-debug && \
    cd llvm-release80-debug && \
    CC=clang CXX=clang++ \
        cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DLLVM_TARGETS_TO_BUILD=host ../llvm-release80/llvm

# Distribute libraries accross separate layers to limit the number of lost
# artifacts if docker-build runs out of memory. The list is obtained from:
# llvm-config --libnames core executionengine native orcjit runtimedyld support
RUN cd llvm-release80-debug && ninja llvm-config FileCheck
RUN cd llvm-release80-debug && ninja LLVMSupport LLVMDemangle LLVMCore LLVMBinaryFormat
RUN cd llvm-release80-debug && ninja LLVMRuntimeDyld LLVMObject LLVMMCParser LLVMBitReader LLVMMC LLVMDebugInfoCodeView LLVMDebugInfoMSF
RUN cd llvm-release80-debug && ninja LLVMExecutionEngine LLVMTarget LLVMAnalysis LLVMProfileData LLVMOrcJIT LLVMTransformUtils
RUN cd llvm-release80-debug && ninja LLVMGlobalISel LLVMSelectionDAG LLVMScalarOpts LLVMInstCombine LLVMAggressiveInstCombine
RUN cd llvm-release80-debug && ninja LLVMAsmPrinter LLVMCodeGen LLVMMCDisassembler LLVMBitWriter LLVMBinaryFormat
RUN cd llvm-release80-debug && ninja LLVMX86AsmPrinter LLVMX86CodeGen LLVMX86Disassembler LLVMX86AsmParser LLVMX86Desc LLVMX86Info LLVMX86Utils
RUN cd llvm-release80-debug && ninja LLVMipo LLVMInstrumentation LLVMVectorize LLVMLinker LLVMIRReader LLVMAsmParser

# Checkout JitFromScratch
RUN git clone https://github.com/weliveindetail/JitFromScratch.git jitfromscratch && \
    cd jitfromscratch && \
    git remote set-url origin --push no-pushing && \
    git checkout llvm80/jit-basics && \
    mkdir ../build

# Expose port for attaching gdb. It must still be published 
# to the host by passing '-p 9999:9999' to docker-run
EXPOSE 9999

# By default: update, build and run JitFromScratch in gdbserver on startup
CMD set -x && \
    cd jitfromscratch && \
    ([ ! "${REMOTE+1}" ] || git remote set-url origin "${REMOTE}") && \
    git fetch --quiet origin && \
    git checkout --quiet FETCH_HEAD && \
    ([ ! "${CHECKOUT+1}" ] || git checkout --quiet "${CHECKOUT}") && \
    git log -1 -s && \
    cd ../build && \
    CC=clang CXX=clang++ \
        cmake -GNinja -DLLVM_DIR=/llvm-release80-debug/lib/cmake/llvm ../jitfromscratch && \
    ninja JitFromScratch && \
    gdbserver :9999 JitFromScratch
