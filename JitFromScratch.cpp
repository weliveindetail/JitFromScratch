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

JitFromScratch::JitFromScratch(ExitOnError ExitOnErr,
                               const std::string &CacheDir)
    : ES(std::make_unique<ExecutionSession>()),
      MainJD(ES->createJITDylib("main")),
      TM(createTargetMachine(ExitOnErr)),
      ObjCache(std::make_unique<SimpleObjectCache>(CacheDir)),
      GDBListener(JITEventListener::createGDBRegistrationListener()),
      ObjLinkingLayer(*ES, createMemoryManagerFtor()),
      CompileLayer(*ES, ObjLinkingLayer, SimpleCompiler(*TM, ObjCache.get())),
      OptimizeLayer(*ES, CompileLayer) {
  ObjLinkingLayer.setNotifyLoaded(createNotifyLoadedFtor());
  if (auto R = createHostProcessResolver())
    MainJD.addGenerator(std::move(R));
}

std::unique_ptr<JITDylib::DefinitionGenerator>
JitFromScratch::createHostProcessResolver() {
  char Prefix = TM->createDataLayout().getGlobalPrefix();
  Expected<std::unique_ptr<DynamicLibrarySearchGenerator>> R =
      DynamicLibrarySearchGenerator::GetForCurrentProcess(Prefix);

  if (!R) {
    ES->reportError(R.takeError());
    return nullptr;
  }

  if (*R == nullptr) {
    ES->reportError(createStringError(
        inconvertibleErrorCode(),
        "Generator function for host process symbols must not be null"));
    return nullptr;
  }

  return std::move(*R);
}

std::unique_ptr<TargetMachine>
JitFromScratch::createTargetMachine(ExitOnError ExitOnErr) {
  auto JTMB = ExitOnErr(JITTargetMachineBuilder::detectHost());
  return ExitOnErr(JTMB.createTargetMachine());
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
    return ObjLinkingLayer.add(MainJD, std::move(Obj->getValue()));
  }

  LLVM_DEBUG(dbgs() << "Submit IR module:\n\n" << *M << "\n\n");

  if (auto Err = applyDataLayout(*M))
    return Err;

  OptimizeLayer.setTransform(SimpleOptimizer(OptLevel));

  return OptimizeLayer.add(MainJD, ThreadSafeModule(std::move(M), std::move(C)),
                           ES->allocateVModule());
}

Expected<JITTargetAddress> JitFromScratch::getFunctionAddr(StringRef Name) {
  JITDylibSearchOrder JDs = makeJITDylibSearchOrder({&MainJD});
  Expected<JITEvaluatedSymbol> S = ES->lookup(JDs, ES->intern(mangle(Name)));
  if (!S)
    return S.takeError();

  JITTargetAddress A = S->getAddress();
  if (!A)
    return createStringError(inconvertibleErrorCode(),
                             "'%s' evaluated to nullptr", Name.data());

  return A;
}
