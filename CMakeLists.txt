cmake_minimum_required(VERSION 3.0)
project(JitFromScratch)

find_package(LLVM 4.0 REQUIRED)
message(STATUS "LLVM_DIR: ${LLVM_DIR}")
message(STATUS "LLVM_PACKAGE_VERSION: ${LLVM_PACKAGE_VERSION}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_definitions(${LLVM_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fno-rtti")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG_DUMP")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEXPLICIT_DEBUG_TRAP")

add_executable(JitFromScratch
    JitFromScratch.cpp
    SimpleOrcJit.h)

llvm_map_components_to_libnames(LLVM_LIBS
    core
    orcjit
    x86asmparser
    x86codegen)

target_link_libraries(JitFromScratch
    ${LLVM_LIBS})

message(STATUS "Include directories: ${CMAKE_CURRENT_SOURCE_DIR};${LLVM_INCLUDE_DIRS}")
message(STATUS "Library directories: ${LLVM_LIBRARY_DIRS}")
message(STATUS "Link libraries: ${LLVM_LIBS}")
message(STATUS "Build flags: ${CMAKE_CXX_FLAGS} ${LLVM_DEFINITIONS}")
message(STATUS "Additional flags Debug: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Additional flags Release: ${CMAKE_CXX_FLAGS_RELEASE}")
