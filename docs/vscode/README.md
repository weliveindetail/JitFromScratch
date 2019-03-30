# JitFromScratch

Collection of examples from my talks in the LLVM Social Berlin and C++ User Group Berlin that showcase techniques for implementing various aspects of a JIT compiler based on the LLVM Orc libraries.

## Build and debug with vscode

Use the extensions [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), [CMake Tools](https://marketplace.visualstudio.com/items?itemName=vector-of-bool.cmake-tools) and [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb) and the configuration files below. Enter your actual `LLVM_DIR` and you should be ready to go.

### .vscode/settings.json
```
{
  "C_Cpp.default.configurationProvider": "vector-of-bool.cmake-tools",
  "cmake.preferredGenerators": [ "Ninja" ]
}
```

### .vscode/cmake-variants.json
```
{
  "LLVM": {
    "default": "LLVM 8.0",
    "choices": {
      "LLVM 8.0": {
        "short": "LLVM 8.0",
        "long": "LLVM Release 8.0",
        "settings": {
          "LLVM_DIR": "/path/to/llvm-build/lib/cmake/llvm"
        }
      }
    }
  },
  "buildType": {
    "default": "debug",
    "choices": {
      "debug": {
        "short": "Debug",
        "long": "Compile in debug mode",
        "buildType": "Debug",
        "linkage": "static"
      }
    }
  }
}
```

### .vscode/launch.json
```
{
  "version": "0.2.0",
  "configurations": [
    {
      "type": "lldb",
      "request": "launch",
      "name": "Launch",
      "program": "${workspaceRoot}/build/JitFromScratch",
      "args": [ "-debug", "-debug-only=jitfromscratch" ],
      "cwd": "${workspaceFolder}/build"
    },
    {
      "type": "gdb",
      "request": "attach",
      "name": "Docker",
      "executable": "${workspaceRoot}/build/JitFromScratch",
      "target": ":9999",
      "remote": true,
      "cwd": "${workspaceRoot}",
      "valuesFormatting": "parseText"
    }
  ]
}
```

Instructions for debugging in a docker container can be found in the [respective dockerfile](https://github.com/weliveindetail/JitFromScratch/blob/master/docs/docker/debug/Dockerfile).
