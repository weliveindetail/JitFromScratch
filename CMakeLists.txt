cmake_minimum_required(VERSION 3.2)

project(JitFromScratch VERSION 1.0 LANGUAGES CXX)

message("")
message("Dependencies")

if(WIN32)
    set(OS_NAME "Win")
    set(DISABLE_RTTI_FLAG /GR-)
elseif(APPLE)
    set(OS_NAME "OSX")
    set(DISABLE_RTTI_FLAG -fno-rtti)
elseif(UNIX)
    set(OS_NAME "Linux")
    set(DISABLE_RTTI_FLAG -fno-rtti)
else()
    message(FATAL_ERROR "Operating system not supported")
endif()
message(STATUS "System: ${OS_NAME}")

add_executable(JitFromScratch
    JitFromScratch.cpp
    SimpleOrcJit.h
)

set_target_properties(JitFromScratch PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# add LLVM libraries, set LLVM_* variables
find_package(LLVM 5.0 REQUIRED PATHS ${LLVM_DIR})
message(STATUS "LLVM_DIR: ${LLVM_DIR}")
message(STATUS "LLVM_PACKAGE_VERSION: ${LLVM_PACKAGE_VERSION}")

# add Clang libraries
include(${LLVM_BUILD_BINARY_DIR}/lib/cmake/clang/ClangTargets.cmake)

if(NOT LLVM_ENABLE_RTTI)
    target_compile_options(JitFromScratch PRIVATE ${DISABLE_RTTI_FLAG})
endif()

if(USE_LLD)
	message(STATUS "Add option to link with LLD")
	target_link_libraries(JitFromScratch PRIVATE "-fuse-ld=lld")
endif()

# find Clang source directory
if(EXISTS ${LLVM_BUILD_MAIN_SRC_DIR}/tools/clang)
    set(CLANG_SRC_DIR_PREFIX tools) # sources in-tree (default build)
elseif(EXISTS ${LLVM_BUILD_MAIN_SRC_DIR}/../clang)
    set(CLANG_SRC_DIR_PREFIX ..) # sources out-of-tree (ENABLE_PROJECTS build) 
else()
    message(FATAL_ERROR "Cannot find Clang sources")
endif()

message(STATUS "LLVM Source Directory: ${LLVM_BUILD_MAIN_SRC_DIR}")
message(STATUS "Clang Source Directory: ${LLVM_BUILD_MAIN_SRC_DIR}/${CLANG_SRC_DIR_PREFIX}/clang")
message(STATUS "LLVM/Clang Build Directory: ${LLVM_BUILD_BINARY_DIR}")

target_include_directories(JitFromScratch PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LLVM_INCLUDE_DIRS}
    ${LLVM_BUILD_BINARY_DIR}/tools/clang/include
    ${LLVM_BUILD_MAIN_SRC_DIR}/${CLANG_SRC_DIR_PREFIX}/clang/include
)

# LLVM definitions
separate_arguments(LLVM_DEFINITIONS)
target_compile_definitions(JitFromScratch PRIVATE
    ${LLVM_DEFINITIONS}
)

# JitFromScratch dependencies
llvm_map_components_to_libnames(LLVM_LIBS
    core
    executionengine
    native
    object
    orcjit
    runtimedyld
    support
)

target_link_libraries(JitFromScratch PRIVATE
    ${LLVM_LIBS}
)

# Clang dependencies
target_link_libraries(JitFromScratch PRIVATE
    LLVMX86CodeGen
    LLVMX86AsmPrinter
    LLVMX86AsmParser
    LLVMX86Desc
    LLVMX86Info
    LLVMX86Disassembler
    LLVMAnalysis
    LLVMCodeGen
    LLVMCore
    LLVMipo
    LLVMInstCombine
    LLVMInstrumentation
    LLVMMC
    LLVMMCParser
    LLVMObjCARCOpts
    LLVMOption
    LLVMScalarOpts
    LLVMSupport
    LLVMTransformUtils
    LLVMVectorize
    clangBasic
    clangCodeGen
    clangDriver
    clangFrontend
    clangFrontendTool
    LLVMAsmPrinter
    LLVMDebugInfoCodeView
    LLVMDebugInfoMSF
    LLVMGlobalISel
    LLVMSelectionDAG
    LLVMX86Utils
    LLVMMCDisassembler
    LLVMCoroutines
    LLVMCoverage
    LLVMLTO
    LLVMPasses
    LLVMTarget
    LLVMBitWriter
    LLVMIRReader
    LLVMAsmParser
    LLVMLinker
    LLVMObject
    clangRewriteFrontend
    clangARCMigrate
    clangStaticAnalyzerFrontend
    LLVMProfileData
    clangParse
    clangSerialization
    LLVMBitReader
    clangSema
    clangEdit
    clangStaticAnalyzerCheckers
    clangASTMatchers
    clangStaticAnalyzerCore
    clangAnalysis
    clangAST
    clangRewrite
    clangLex
    LLVMDemangle
)

message("")
message("JitFromScratch")

get_target_property(ALL_INCLUDE_DIRECTORIES JitFromScratch INCLUDE_DIRECTORIES)
message(STATUS "Include directories: ${ALL_INCLUDE_DIRECTORIES}")

get_target_property(ALL_LINK_LIBRARIES JitFromScratch LINK_LIBRARIES)
message(STATUS "Link libraries: ${ALL_LINK_LIBRARIES}")

get_target_property(ALL_COMPILE_OPTIONS JitFromScratch COMPILE_OPTIONS)
message(STATUS "Compile options: ${ALL_COMPILE_OPTIONS}")

get_target_property(ALL_COMPILE_DEFINITIONS JitFromScratch COMPILE_DEFINITIONS)
message(STATUS "Compile definitions: ${ALL_COMPILE_DEFINITIONS}")

message(STATUS "Other flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Other flags Debug: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Other flags Release: ${CMAKE_CXX_FLAGS_RELEASE}")
