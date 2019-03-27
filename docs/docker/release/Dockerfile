# JitFromScratch docker image for build against official LLVM 9 Release
#
# The image ships the LLVM 9 Release version installed from the LLVM apt
# repository and a checkout of the JitFromScratch sources. By default the
# container will build the llvm09/master branch with GCC. Passing values
# for the environment variables REMOTE and CHECKOUT allows to test arbitrary
# forks and/or revisions.
#
#   $ docker build -t <image-name> /path/to/JitFromScratch/docker/release
#   $ docker run -e REMOTE=<repo> -e CHECKOUT=<commit> <image-name>
#
FROM ubuntu:19.04
LABEL maintainer="weliveindetail <stefan.graenitz@gmail.com>"

# Install tools for apt-add-repository
RUN apt-get update && apt-get install -y --no-install-recommends \
            ca-certificates wget gnupg software-properties-common && \
    rm -rf /var/lib/apt/lists/*

# Add LLVM apt repository
RUN wget https://apt.llvm.org/llvm-snapshot.gpg.key && \
    apt-key add llvm-snapshot.gpg.key && \
    apt-add-repository "deb http://apt.llvm.org/disco/ llvm-toolchain-disco-9 main" && \
    apt-get update

# Install tools and libs for JitFromScratch build
RUN apt-get install -y --no-install-recommends \
            build-essential git cmake cmake-data \
            ninja-build zlib1g-dev llvm-9-dev && \
    rm -rf /var/lib/apt/lists/*

# Checkout JitFromScratch
RUN git clone https://github.com/weliveindetail/JitFromScratch.git \
              /home/jitfromscratch && \
    cd /home/jitfromscratch && \
    git checkout llvm09/master

# By default: update, build and run on startup
CMD set -x && \
    cd /home/jitfromscratch && \
    ([ ! "${REMOTE+1}" ] || git remote set-url origin "${REMOTE}") && \
    git fetch --quiet origin && \
    git checkout --quiet FETCH_HEAD && \
    ([ ! "${CHECKOUT+1}" ] || git checkout --quiet "${CHECKOUT}") && \
    git log -1 -s && \
    cmake -G Ninja -S /home/jitfromscratch -B /home/jitfromscratch-release \
      -DCMAKE_BUILD_TYPE=Release && \
    ninja -C /home/jitfromscratch-release -v run
