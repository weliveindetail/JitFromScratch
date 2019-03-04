#pragma once

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Error.h>
#include <llvm/Target/TargetMachine.h>

#include <memory>

class JitFromScratch : public llvm::orc::LLJIT {
public:
  JitFromScratch(std::unique_ptr<llvm::TargetMachine> TM, llvm::DataLayout DL);

  // Not a value type.
  JitFromScratch(const JitFromScratch &) = delete;
  JitFromScratch &operator=(const JitFromScratch &) = delete;
  JitFromScratch(JitFromScratch &&) = delete;
  JitFromScratch &operator=(JitFromScratch &&) = delete;

  llvm::Error submitModule(std::unique_ptr<llvm::Module> M,
                           std::unique_ptr<llvm::LLVMContext> C);
};
