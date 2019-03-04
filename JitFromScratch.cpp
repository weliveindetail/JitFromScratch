#include "JitFromScratch.h"

#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/Support/Debug.h>

#define DEBUG_TYPE "jitfromscratch"

using namespace llvm;
using namespace llvm::orc;

JitFromScratch::JitFromScratch(std::unique_ptr<TargetMachine> TM, DataLayout DL)
    : LLJIT(std::make_unique<ExecutionSession>(), std::move(TM), DL) {
  if (auto R = createHostProcessResolver(DL))
    Main.setGenerator(std::move(R));
}

JITDylib::GeneratorFunction
JitFromScratch::createHostProcessResolver(DataLayout DL) {
  Expected<JITDylib::GeneratorFunction> R =
      DynamicLibrarySearchGenerator::GetForCurrentProcess(DL);

  if (!R) {
    ES->reportError(R.takeError());
    return nullptr;
  }

  if (!*R) {
    ES->reportError(createStringError(
        inconvertibleErrorCode(),
        "Generator function for host process symbols must not be null"));
    return nullptr;
  }

  return *R;
}

Error JitFromScratch::submitModule(std::unique_ptr<Module> M,
                                   std::unique_ptr<LLVMContext> C) {
  LLVM_DEBUG(dbgs() << "Submit IR module:\n\n" << *M << "\n\n");
  return addIRModule(ThreadSafeModule(std::move(M), std::move(C)));
}

Expected<JITTargetAddress> JitFromScratch::getFunctionAddr(StringRef Name) {
  Expected<JITEvaluatedSymbol> S = lookup(Name);
  if (!S)
    return S.takeError();

  JITTargetAddress A = S->getAddress();
  if (!A)
    return createStringError(inconvertibleErrorCode(),
                             "'%s' evaluated to nullptr", Name.data());

  return A;
}
