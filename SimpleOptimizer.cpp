#include "SimpleOptimizer.h"

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/raw_ostream.h>

#define DEBUG_TYPE "jitfromscratch"

using namespace llvm;
using namespace llvm::orc;

Expected<ThreadSafeModule>
SimpleOptimizer::operator()(ThreadSafeModule TSM,
                            const MaterializationResponsibility &) {
  TSM.withModuleDo([&](Module &M) {
    legacy::FunctionPassManager FPM(&M);
    B.populateFunctionPassManager(FPM);

    FPM.doInitialization();
    for (Function &F : M)
      FPM.run(F);
    FPM.doFinalization();

    legacy::PassManager MPM;
    B.populateModulePassManager(MPM);
    MPM.run(M);

    LLVM_DEBUG(dbgs() << "Optimized IR module:\n\n" << M << "\n\n");
  });

  return std::move(TSM);
}
