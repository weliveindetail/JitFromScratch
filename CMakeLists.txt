cmake_minimum_required(VERSION 3.10)

project(JitFromScratch)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

if(NOT CMAKE_TOOLCHAIN_FILE)
  include(DefaultToolchain)
endif()

add_executable(JitFromScratch
  main.cpp
)

target_include_directories(JitFromScratch PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

function(dump_target_properties target property)
  get_target_property(values ${target} ${property})
  if(values)
    if(NOT "${values}" STREQUAL "${property}-NOTFOUND")
      message(STATUS "${target} ${property}=${values}")
    endif()
  endif()
endfunction()

dump_target_properties(JitFromScratch INCLUDE_DIRECTORIES)
dump_target_properties(JitFromScratch COMPILE_OPTIONS)
dump_target_properties(JitFromScratch COMPILE_DEFINITIONS)
dump_target_properties(JitFromScratch LINK_LIBRARIES)
dump_target_properties(JitFromScratch LINK_FLAGS)

add_custom_target(run
  COMMAND $<TARGET_FILE:JitFromScratch>
  COMMENT "Running JitFromScratch"
)

message("")
