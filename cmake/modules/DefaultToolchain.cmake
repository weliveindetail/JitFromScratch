message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Arch: ${CMAKE_SYSTEM_PROCESSOR}")

function(get_version_short path version_cmd result)
  if(path)
    execute_process(
      COMMAND ${path} ${version_cmd}
      RESULT_VARIABLE exit_code
      OUTPUT_VARIABLE full_version
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )
    if(full_version AND "${exit_code}" STREQUAL 0)
      string(REGEX REPLACE "\n" ";" lines ${full_version})
      list(GET lines 0 first_line)
      set(${result} ${first_line} PARENT_SCOPE)
      return()
    endif()
  endif()
  set(${result} "${result}-NOTFOUND" PARENT_SCOPE)
endfunction()

# Check for Clang
if(CMAKE_C_COMPILER_ID STREQUAL Clang AND
   CMAKE_CXX_COMPILER_ID STREQUAL Clang)
  set(using_clang ON)
elseif(CMAKE_C_COMPILER_ID STREQUAL AppleClang AND
       CMAKE_CXX_COMPILER_ID STREQUAL AppleClang)
  set(using_clang ON)
endif()

# Check for LLD
if(using_clang)
  execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -print-prog-name=ld.lld
    OUTPUT_VARIABLE lld_path
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if(EXISTS "${lld_path}")
    set(CMAKE_LINKER ${lld_path})
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
  endif()
endif()

# Project-specific settings
if(UNIX OR using_clang)
  set(HOST_ENABLE_RTTI)
  set(HOST_DISABLE_RTTI -fno-rtti -fno-exceptions)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else()
  set(HOST_ENABLE_RTTI)
  set(HOST_DISABLE_RTTI /GR- /EHs- /EHc-)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14")
endif()

# Print results
if(using_clang)
  get_version_short(${CMAKE_CXX_COMPILER} --version clang_version)
  message(STATUS "Compiler: ${clang_version}")
else()
  message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_C_COMPILER}, ${CMAKE_CXX_COMPILER})")
endif()

get_version_short(${CMAKE_LINKER} -v linker_version)
if(linker_version)
  message(STATUS "Linker: ${linker_version}")
else()
  message(STATUS "Linker: ${CMAKE_LINKER}")
endif()

message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
