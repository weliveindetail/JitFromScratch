#pragma once

#include <llvm/ADT/StringRef.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/ObjectCache.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/IRTransformLayer.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/Error.h>
#include <llvm/Target/TargetMachine.h>

#include <functional>
#include <memory>
#include <string>

#include "SimpleObjectCache.h"

class JitFromScratch {
public:
  JitFromScratch(std::unique_ptr<llvm::TargetMachine> TM,
                 const std::string &CacheDir = "");

  // Not a value type.
  JitFromScratch(const JitFromScratch &) = delete;
  JitFromScratch &operator=(const JitFromScratch &) = delete;
  JitFromScratch(JitFromScratch &&) = delete;
  JitFromScratch &operator=(JitFromScratch &&) = delete;

  llvm::Error submitModule(std::unique_ptr<llvm::Module> M,
                           std::unique_ptr<llvm::LLVMContext> C,
                           unsigned OptLevel, bool AddToCache);

  template <class Signature_t>
  llvm::Expected<std::function<Signature_t>> getFunction(llvm::StringRef Name) {
    if (auto A = getFunctionAddr(Name))
      return std::function<Signature_t>(
          llvm::jitTargetAddressToPointer<Signature_t *>(*A));
    else
      return A.takeError();
  }

private:
  std::unique_ptr<llvm::orc::ExecutionSession> ES;
  std::unique_ptr<llvm::TargetMachine> TM;

  std::unique_ptr<SimpleObjectCache> ObjCache;
  llvm::JITEventListener *GDBListener;

  llvm::orc::RTDyldObjectLinkingLayer ObjLinkingLayer;
  llvm::orc::IRCompileLayer CompileLayer;
  llvm::orc::IRTransformLayer OptimizeLayer;

  llvm::orc::JITDylib::GeneratorFunction createHostProcessResolver();

  llvm::orc::RTDyldObjectLinkingLayer::GetMemoryManagerFunction
  createMemoryManagerFtor();

  llvm::orc::RTDyldObjectLinkingLayer::NotifyLoadedFunction
  createNotifyLoadedFtor();

  std::string mangle(llvm::StringRef UnmangledName);
  llvm::Error applyDataLayout(llvm::Module &M);

  llvm::Expected<llvm::JITTargetAddress> getFunctionAddr(llvm::StringRef Name);
};
