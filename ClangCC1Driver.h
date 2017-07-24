#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Error.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

class ClangCC1Driver {
public:
  ClangCC1Driver() = default;

  // As long as the driver exists, source files remain on disk to allow
  // debugging JITed code.
  ~ClangCC1Driver() {
    for (auto D : SoucreFileDeleters)
      D();
  }

  llvm::Expected<std::unique_ptr<llvm::Module>>
  compileTranslationUnit(std::string cppCode, llvm::LLVMContext &context);

private:
  std::vector<std::function<void()>> SoucreFileDeleters;
};
