#pragma once

#include <llvm/ExecutionEngine/Orc/LLJIT.h>

#include <memory>

class JitFromScratch {
public:
  JitFromScratch(llvm::ExitOnError ExitOnErr);

  // Not a value type.
  JitFromScratch(const JitFromScratch &) = delete;
  JitFromScratch &operator=(const JitFromScratch &) = delete;
  JitFromScratch(JitFromScratch &&) = delete;
  JitFromScratch &operator=(JitFromScratch &&) = delete;

private:
  std::unique_ptr<llvm::orc::LLJIT> LLJIT;
};
