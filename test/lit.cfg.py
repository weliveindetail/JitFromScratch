import os

import lit.util
import lit.formats
from lit.llvm import llvm_config

config.name = 'JitFromScratch'

config.test_format = lit.formats.ShTest()
config.test_source_root = os.path.dirname(__file__)
config.suffixes = ['.test']

if config.build_type.lower() == "debug":
  config.suffixes.append('.test-debug')

# Add binary directories for JitFromScratch and FileCheck executables
llvm_config.with_environment('PATH', config.jitfromscratch_build_dir, append_path=True)
llvm_config.with_environment('PATH', config.llvm_tools_dir, append_path=True)
