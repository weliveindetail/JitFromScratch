#include <string>
#include <vector>

#include <llvm/ADT/StringRef.h>

#define STRINGIFY_DETAIL(X) #X
#define STRINGIFY(X) STRINGIFY_DETAIL(X)

std::vector<std::string> getClangCC1Args(llvm::StringRef cpp,
                                         llvm::StringRef bc) {
  std::vector<std::string> args;

  args.push_back("-emit-llvm");
  args.push_back("-emit-llvm-bc");
  args.push_back("-emit-llvm-uselists");

  args.push_back("-main-file-name");
  args.push_back(cpp.data());

  args.push_back("-std=c++14");
  args.push_back("-disable-free");

  args.push_back("-fmath-errno");
  args.push_back("-fdeprecated-macro");

  args.push_back("-fms-extensions");
  args.push_back("-fms-compatibility");
  args.push_back("-fms-compatibility-version=18");
  args.push_back("-fno-threadsafe-statics");
  args.push_back("-fdelayed-template-parsing");

  args.push_back("-mrelocation-model");
  args.push_back("pic");
  args.push_back("-pic-level");
  args.push_back("2");
  args.push_back("-mthread-model");
  args.push_back("posix");
  args.push_back("-masm-verbose");
  args.push_back("-mconstructor-aliases");
  args.push_back("-munwind-tables");
  args.push_back("-momit-leaf-frame-pointer");

  args.push_back("-dwarf-column-info");
  args.push_back("-debugger-tuning=gdb");

#if NDEBUG
  args.push_back("-O3");
  args.push_back("-vectorize-loops");
  args.push_back("-vectorize-slp");
#else
  args.push_back("-debug-info-kind=standalone");
  args.push_back("-dwarf-version=4");
#endif

  args.push_back("-resource-dir");
  args.push_back(STRINGIFY(JIT_FROM_SCRATCH_CLANG_RESOURCE_DIR));

  args.push_back("-internal-isystem");
  args.push_back(STRINGIFY(JIT_FROM_SCRATCH_CLANG_RESOURCE_DIR) "/include");
  /*
  "-internal-isystem"
  "D:\\bin\\..\\lib\\clang\\4.0.1\\include"
  "-internal-isystem"
  "C:\\Program Files (x86)\\Microsoft Visual
  Studio\\2017\\Community\\VC\\Tools\\MSVC\\14.10.25017\\ATLMFC\\include"
  "-internal-isystem"
  "C:\\Program Files (x86)\\Microsoft Visual
  Studio\\2017\\Community\\VC\\Tools\\MSVC\\14.10.25017\\include"
  "-internal-isystem"
  "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.15063.0\\ucrt"
  "-internal-isystem"
  "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.15063.0\\shared"
  "-internal-isystem"
  "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.15063.0\\um"
  "-internal-isystem"
  "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.15063.0\\winrt"
  */

  args.push_back("-o");
  args.push_back(bc.data());
  args.push_back("-x");
  args.push_back("c++");
  args.push_back(cpp.data());

  return args;
}

#undef STRINGIFY
#undef STRINGIFY_DETAIL
