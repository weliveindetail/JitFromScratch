#include "JitFromScratch.h"

#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/Support/Debug.h>

#define DEBUG_TYPE "jitfromscratch"

using namespace llvm;
using namespace llvm::orc;

JitFromScratch::JitFromScratch(ExitOnError ExitOnErr) {
  LLJITBuilder Builder;
  ExitOnErr(Builder.prepareForConstruction());
  TT = Builder.JTMB->getTargetTriple();
  LLJIT = ExitOnErr(Builder.create());

  if (auto R = createHostProcessResolver(LLJIT->getDataLayout()))
    LLJIT->getMainJITDylib().setGenerator(std::move(R));
}

JITDylib::GeneratorFunction
JitFromScratch::createHostProcessResolver(DataLayout DL) {
  char Prefix = DL.getGlobalPrefix();
  Expected<JITDylib::GeneratorFunction> R =
      DynamicLibrarySearchGenerator::GetForCurrentProcess(Prefix);

  if (!R) {
    LLJIT->getExecutionSession().reportError(R.takeError());
    return nullptr;
  }

  if (!*R) {
    LLJIT->getExecutionSession().reportError(createStringError(
        inconvertibleErrorCode(),
        "Generator function for host process symbols must not be null"));
    return nullptr;
  }

  return *R;
}

Error JitFromScratch::submitModule(std::unique_ptr<Module> M,
                                   std::unique_ptr<LLVMContext> C) {
  LLVM_DEBUG(dbgs() << "Submit IR module:\n\n" << *M << "\n\n");
  return LLJIT->addIRModule(ThreadSafeModule(std::move(M), std::move(C)));
}

Expected<JITTargetAddress> JitFromScratch::getFunctionAddr(StringRef Name) {
  Expected<JITEvaluatedSymbol> S = LLJIT->lookup(Name);
  if (!S)
    return S.takeError();

  JITTargetAddress A = S->getAddress();
  if (!A)
    return createStringError(inconvertibleErrorCode(),
                             "'%s' evaluated to nullptr", Name.data());

  return A;
}
