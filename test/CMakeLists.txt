# Used in add_lit_target to find the llvm-lit driver
set(LLVM_RUNTIME_OUTPUT_INTDIR ${LLVM_TOOLS_BINARY_DIR})

# Show test output for failures
list(APPEND LLVM_LIT_ARGS -v)

# lit driver and config files are Python scripts
set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 2.7)
include(FindPythonInterp)

if(NOT PYTHONINTERP_FOUND OR ${PYTHON_VERSION_STRING} VERSION_LESS 2.7)
  string(CONCAT error
    "Python 2.7 or newer is required for tests. Please install "
    "Python or specify the PYTHON_EXECUTABLE CMake variable. "
  )
endif()

if(NOT TARGET FileCheck)
  if(LLVM_BUILD_MAIN_SRC_DIR)
    string(CONCAT error ${error}
      "FileCheck is required for tests. Make sure the "
      "${LLVM_TOOLS_BINARY_DIR}/FileCheck executable was built. "
      "Make sure LLVM has LLVM_BUILD_UTILS=ON (default). "
    )
  else()
    string(CONCAT error ${error}
      "The installed LLVM does not provide the FileCheck executable. This is "
      "the default for distribution packages (the distributor can change that "
      "by passing LLVM_INSTALL_UTILS=ON). Without FileCheck lit tests "
      "cannot run. Build LLVM yourself from source and point JitFromScratch to "
      "the build-tree to enable tests. "
    )
  endif()
endif()

if(error)
  message(WARNING "Failed to configure tests: ${error}")
  add_custom_target(check
    COMMAND ${CMAKE_COMMAND} -E echo
    COMMENT "Cannot run tests"
  )
  # We still want to build the executable.
  add_dependencies(check JitFromScratch)
else()
  set(LLVM_DEFAULT_EXTERNAL_LIT ${LLVM_TOOLS_BINARY_DIR}/llvm-lit
      CACHE PATH "Path to llvm-lit")

  configure_lit_site_cfg(
    ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg.py
    MAIN_CONFIG
    ${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg.py
  )

  add_lit_target(check "Running tests"
    ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS JitFromScratch FileCheck
  )
endif()

message("")
