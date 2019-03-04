#pragma once

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IR/DataLayout.h>
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
};
