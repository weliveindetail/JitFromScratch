#include "JitFromScratch.h"

#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>

using namespace llvm;
using namespace llvm::orc;

JitFromScratch::JitFromScratch(ExitOnError ExitOnErr)
    : LLJIT(ExitOnErr(LLJITBuilder().create())) {}

Error JitFromScratch::submitModule(std::unique_ptr<Module> M,
                                   std::unique_ptr<LLVMContext> C) {
  return LLJIT->addIRModule(ThreadSafeModule(std::move(M), std::move(C)));
}
