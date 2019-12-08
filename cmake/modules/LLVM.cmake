if(LLVM_DIR)
  file(TO_CMAKE_PATH ${LLVM_DIR} LLVM_DIR)
  message(STATUS "Configure with explicit LLVM_DIR=${LLVM_DIR}")
else()
  message(STATUS
    "No explicit LLVM_DIR provided. If find_package() fails, pass "
    "-DLLVM_DIR=/path/to/llvm-build/lib/cmake/llvm to CMake.")
endif()

find_package(LLVM 9.0 REQUIRED PATHS /usr/local/opt/llvm/lib/cmake/llvm)
message(STATUS "LLVM Package version: ${LLVM_PACKAGE_VERSION}")

if(LLVM_BUILD_MAIN_SRC_DIR)
  message(STATUS "LLVM Source directory: ${LLVM_BUILD_MAIN_SRC_DIR}")
  message(STATUS "LLVM Build directory: ${LLVM_BINARY_DIR}")
else()
  message(STATUS "LLVM Install directory: ${LLVM_BINARY_DIR}")
endif()

message(STATUS "LLVM Build type: ${LLVM_BUILD_TYPE}")
