# JitFromScratch

Collection of examples from my talks in the LLVM Social Berlin and C++ User Group Berlin that showcase techniques for implementing various aspects of a JIT compiler based on the LLVM Orc libraries.

## Dockerfiles

Each of the below Dockerfiles defines an image that ships a prebuilt version of LLVM 9 and a checkout of the JitFromScratch sources in Ubuntu 19.04 (disco).
Containers instantiated from an image pull the `llvm09/master` branch from [this remote](https://github.com/weliveindetail/JitFromScratch.git). In order to build another fork and/or revision of the project, pass the environment variables REMOTE and/or CHECKOUT as explained in the Dockerfiles' comments.
Note that these images can also be [pulled from dockerhub](https://cloud.docker.com/u/weliveindetail/repository/docker/weliveindetail/jitfromscratch).

### debug

Ships a prebuilt debug version of LLVM 9. Containers (remote host) run lldb-server in platform mode and accept connections on port 9000. Recent security improvements in the Linux kernel require extra options to be passed to docker-run in order to attach a debugger from the outside (local host).

Run remote host in docker:
```
$ docker run --privileged --cap-add=SYS_PTRACE \
             --security-opt seccomp=unconfined \
             --security-opt apparmor=unconfined \
             -p 9000-9001:9000-9001 weliveindetail/jitfromscratch:llvm09-debug
```

Debug from local host:
```
$ lldb
(lldb) platform select remote-linux
(lldb) platform connect connect://localhost:9000
(lldb) target create /workspace/jitfromscratch-debug/JitFromScratch
(lldb) settings set target.source-map \
/workspace/jitfromscratch /path/to/jitfromscratch \
/workspace/llvm-project /path/to/jitfromscratch/build/llvm-project
(lldb) b main
Breakpoint 1: where = JitFromScratch`main + 26 at main.cpp:135, address = 0x0000000000b0ffaa
(lldb) process launch
   ...
   134  int main(int argc, char **argv) {
-> 135    InitLLVM X(argc, argv);
   ...
```

### release

Ships the LLVM 9 release version installed from the [official apt repository](https://apt.llvm.org). Containers build and run the JitFromScratch executable:
```
$ docker run weliveindetail/jitfromscratch:llvm09-release
```

### test-debug

Ships a prebuilt debug version of LLVM 9. Containers build and run the JitFromScratch executable and lit tests:
```
$ docker run weliveindetail/jitfromscratch:llvm09-test-debug
```

### test-release

Ships a prebuilt release version of LLVM 9. Containers build and run the JitFromScratch executable and lit tests:
```
$ docker run weliveindetail/jitfromscratch:llvm09-test-release
```
