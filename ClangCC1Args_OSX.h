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
  args.push_back("-stack-protector");
  args.push_back("1");
  args.push_back("-fmax-type-align=16");

  args.push_back("-mrelocation-model");
  args.push_back("pic");
  args.push_back("-pic-level");
  args.push_back("2");
  args.push_back("-mthread-model");
  args.push_back("posix");
  args.push_back("-mdisable-fp-elim");
  args.push_back("-masm-verbose");
  args.push_back("-munwind-tables");

  args.push_back("-dwarf-column-info");
  args.push_back("-debugger-tuning=lldb");

#if NDEBUG
  args.push_back("-O3");
  args.push_back("-vectorize-loops");
  args.push_back("-vectorize-slp");
#else
  args.push_back("-debug-info-kind=standalone");
  args.push_back("-dwarf-version=4");
#endif

  args.push_back("-stdlib=libc++");
  args.push_back("-resource-dir");
  args.push_back(STRINGIFY(JIT_FROM_SCRATCH_CLANG_RESOURCE_DIR));

  args.push_back("-o");
  args.push_back(bc.data());
  args.push_back("-x");
  args.push_back("c++");
  args.push_back(cpp.data());

  return args;
}

#undef STRINGIFY
#undef STRINGIFY_DETAIL
