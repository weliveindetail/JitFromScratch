# JitFromScratch docker image for build against official LLVM 8 Release
#
# The image ships the LLVM 8 Release version installed from the LLVM apt
# repository and a checkout of the JitFromScratch sources. By default the
# container will build the llvm80/jit-basics branch with GCC. Passing values
# for the environment variables REMOTE and CHECKOUT allows to test arbitrary
# forks and/or revisions.
#
#   $ docker build -t <image-name> /path/to/JitFromScratch/docker/release
#   $ docker run -e REMOTE=<repo> -e CHECKOUT=<commit> <image-name>
#
FROM ubuntu:18.04
LABEL maintainer "weliveindetail <stefan.graenitz@gmail.com>"

# Install tools for apt-add-repository
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
            ca-certificates wget gnupg software-properties-common

# Add LLVM apt repository
RUN wget https://apt.llvm.org/llvm-snapshot.gpg.key && \
    apt-key add llvm-snapshot.gpg.key && \
    apt-add-repository "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-8 main" && \
    apt-get update

# Install tools and libs for JitFromScratch build
RUN apt-get install -y --no-install-recommends \
            build-essential git cmake cmake-data \
            ninja-build zlib1g-dev llvm-8-dev

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
    cmake -GNinja -DLLVM_DIR=/usr/lib/llvm-8/lib/cmake/llvm ../jitfromscratch && \
    ninja run
