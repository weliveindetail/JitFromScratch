#include "JitFromScratch.h"
#include "SimpleOptimizer.h"

#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/Mangler.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

#define DEBUG_TYPE "jitfromscratch"

using namespace llvm;
using namespace llvm::orc;

JitFromScratch::JitFromScratch(std::unique_ptr<TargetMachine> TM,
                               const std::string &CacheDir)
    : ES(std::make_unique<ExecutionSession>()), TM(std::move(TM)),
      ObjCache(std::make_unique<SimpleObjectCache>(CacheDir)),
      GDBListener(JITEventListener::createGDBRegistrationListener()),
      ObjLinkingLayer(*ES, createMemoryManagerFtor(), createNotifyLoadedFtor()),
      CompileLayer(*ES, ObjLinkingLayer,
                   SimpleCompiler(*this->TM, ObjCache.get())),
      OptimizeLayer(*ES, CompileLayer) {
  if (auto R = createHostProcessResolver())
    ES->getMainJITDylib().setGenerator(std::move(R));
}

JITDylib::GeneratorFunction JitFromScratch::createHostProcessResolver() {
  DataLayout DL = TM->createDataLayout();
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

using GetMemoryManagerFunction =
    RTDyldObjectLinkingLayer::GetMemoryManagerFunction;

GetMemoryManagerFunction JitFromScratch::createMemoryManagerFtor() {
  return []() -> GetMemoryManagerFunction::result_type {
    return std::make_unique<SectionMemoryManager>();
  };
}

RTDyldObjectLinkingLayer::NotifyLoadedFunction
JitFromScratch::createNotifyLoadedFtor() {
  using namespace std::placeholders;
  return std::bind(&JITEventListener::notifyObjectLoaded,
                   GDBListener, _1, _2, _3);
}

std::string JitFromScratch::mangle(StringRef UnmangledName) {
  std::string MangledName;
  {
    DataLayout DL = TM->createDataLayout();
    raw_string_ostream MangledNameStream(MangledName);
    Mangler::getNameWithPrefix(MangledNameStream, UnmangledName, DL);
  }
  return MangledName;
}

Error JitFromScratch::applyDataLayout(Module &M) {
  DataLayout DL = TM->createDataLayout();
  if (M.getDataLayout().isDefault())
    M.setDataLayout(DL);

  if (M.getDataLayout() != DL)
    return make_error<StringError>(
        "Added modules have incompatible data layouts",
        inconvertibleErrorCode());

  return Error::success();
}

Error JitFromScratch::submitModule(std::unique_ptr<Module> M,
                                   std::unique_ptr<LLVMContext> C,
                                   unsigned OptLevel, bool AddToCache) {
  if (AddToCache)
    ObjCache->setCacheModuleName(*M);

  auto Obj = ObjCache->getCachedObject(*M);
  if (!Obj) {
    M.~unique_ptr();
    return Obj.takeError();
  }

  if (Obj->hasValue()) {
    M.~unique_ptr();
    return ObjLinkingLayer.add(ES->getMainJITDylib(),
                               std::move(Obj->getValue()));
  }

  LLVM_DEBUG(dbgs() << "Submit IR module:\n\n" << *M << "\n\n");

  if (auto Err = applyDataLayout(*M))
    return Err;

  OptimizeLayer.setTransform(SimpleOptimizer(OptLevel));

  return OptimizeLayer.add(ES->getMainJITDylib(),
                           ThreadSafeModule(std::move(M), std::move(C)),
                           ES->allocateVModule());
}

Expected<JITTargetAddress> JitFromScratch::getFunctionAddr(StringRef Name) {
  SymbolStringPtr NamePtr = ES->intern(mangle(Name));
  JITDylibSearchList JDs{{&ES->getMainJITDylib(), true}};

  Expected<JITEvaluatedSymbol> S = ES->lookup(JDs, NamePtr);
  if (!S)
    return S.takeError();

  JITTargetAddress A = S->getAddress();
  if (!A)
    return createStringError(inconvertibleErrorCode(),
                             "'%s' evaluated to nullptr", Name.data());

  return A;
}
