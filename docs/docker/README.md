# JitFromScratch

Collection of examples from my talks in the LLVM Social Berlin and C++ User Group Berlin that showcase techniques for implementing various aspects of a JIT compiler based on the LLVM Orc libraries.

## Dockerfiles

Each of the below Dockerfiles defines a docker image that ships a prebuilt version of LLVM 8 and a checkout of the JitFromScratch sources.
Containers instantiated from the images pull the `master` branch from [this](https://github.com/weliveindetail/JitFromScratch.git) repository. In order to use another fork and/or revision of the project, pass the environment variables REMOTE and/or CHECKOUT.
Note that these images can also be pulled from [dockerhub](https://cloud.docker.com/u/weliveindetail/repository/docker/weliveindetail/jitfromscratch):

### debug

Ships a prebuilt debug version of LLVM 8. Containers (remote host) run the JitFromScratch executable in gdbserver and accept connections on port 9999. Recent security improvements in the Linux kernel require extra options to be passed to docker-run.

Run remote host:
```
$ docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined --security-opt apparmor=unconfined \
             -p 9999:9999 -e REMOTE=<repo> -e CHECKOUT=<commit> weliveindetail/jitfromscratch:debug
```

Debug from local host:
```
$ gdb -q
$ (gdb) directory /path/to/JitFromScratch-src
$ (gdb) directory /path/to/llvm-src/..
$ (gdb) target remote :9999
$ (gdb) b main
$ (gdb) c
...
Breakpoint 1, main (argc=1, argv=0x7fffffffe788) at /JitFromScratch/main.cpp:109
109	int main(int argc, char **argv) {
...
$ (gdb) monitor exit
```

### release

Ships the LLVM 8 release version installed from the [LLVM apt repository](https://apt.llvm.org). Containers build and run the JitFromScratch executable:
```
$ docker run -e REMOTE=<repo> -e CHECKOUT=<commit> weliveindetail/jitfromscratch:release
```

### test-debug

Ships a prebuilt debug version of LLVM 8. Containers build and run the JitFromScratch executable and lit tests:
```
$ docker run -e REMOTE=<repo> -e CHECKOUT=<commit> weliveindetail/jitfromscratch:test-debug
```

### test-release

Ships a prebuilt release version of LLVM 8. Containers build and run the JitFromScratch executable and lit tests:
```
$ docker run -e REMOTE=<repo> -e CHECKOUT=<commit> weliveindetail/jitfromscratch:test-release
```
