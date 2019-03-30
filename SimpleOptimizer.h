#pragma once

#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/Support/Error.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

class SimpleOptimizer {
public:
  SimpleOptimizer(unsigned OptLevel) { B.OptLevel = OptLevel; }

  llvm::Expected<llvm::orc::ThreadSafeModule>
  operator()(llvm::orc::ThreadSafeModule TSM,
             const llvm::orc::MaterializationResponsibility &);

private:
  llvm::PassManagerBuilder B;
};
