#pragma once

#include <llvm/ADT/Triple.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Error.h>

#include <memory>

class JitFromScratch {
public:
  JitFromScratch(llvm::ExitOnError ExitOnErr);

  // Not a value type.
  JitFromScratch(const JitFromScratch &) = delete;
  JitFromScratch &operator=(const JitFromScratch &) = delete;
  JitFromScratch(JitFromScratch &&) = delete;
  JitFromScratch &operator=(JitFromScratch &&) = delete;

  llvm::DataLayout getDataLayout() const {
    return LLJIT->getDataLayout();
  }

  const llvm::Triple &getTargetTriple() const {
    return TT;
  }

  llvm::Error submitModule(std::unique_ptr<llvm::Module> M,
                           std::unique_ptr<llvm::LLVMContext> C);
private:
  std::unique_ptr<llvm::orc::LLJIT> LLJIT;
  llvm::Triple TT;
};
