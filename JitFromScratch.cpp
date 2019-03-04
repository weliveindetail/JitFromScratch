#include "JitFromScratch.h"

#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>

using namespace llvm;
using namespace llvm::orc;

JitFromScratch::JitFromScratch(std::unique_ptr<TargetMachine> TM, DataLayout DL)
    : LLJIT(std::make_unique<ExecutionSession>(), std::move(TM), DL) {}

Error JitFromScratch::submitModule(std::unique_ptr<Module> M,
                                   std::unique_ptr<LLVMContext> C) {
  return addIRModule(ThreadSafeModule(std::move(M), std::move(C)));
}
