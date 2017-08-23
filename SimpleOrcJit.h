#pragma once

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/NullResolver.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>

#include <memory>
#include <vector>

class SimpleOrcJit {
  using ModulePtr_t = std::unique_ptr<llvm::Module>;
  using IRCompiler_t = llvm::orc::SimpleCompiler;

  using ObjectLayer_t = llvm::orc::RTDyldObjectLinkingLayer;
  using CompileLayer_t = llvm::orc::IRCompileLayer<ObjectLayer_t, IRCompiler_t>;

public:
  SimpleOrcJit(llvm::TargetMachine &targetMachine)
      : DL(targetMachine.createDataLayout()),
        MemoryManagerPtr(std::make_shared<llvm::SectionMemoryManager>()),
        SymbolResolverPtr(std::make_shared<llvm::orc::NullResolver>()),
        ObjectLayer([this]() { return MemoryManagerPtr; }),
        CompileLayer(ObjectLayer, IRCompiler_t(targetMachine)) {}

  void submitModule(ModulePtr_t module) {
    // Commit module for compilation to machine code. Actual compilation
    // happens on demand as soon as one of it's symbols is accessed. None of
    // the layers used here issue Errors from this call.
    llvm::cantFail(
        CompileLayer.addModule(std::move(module), SymbolResolverPtr));
  }

private:
  llvm::DataLayout DL;
  std::shared_ptr<llvm::RTDyldMemoryManager> MemoryManagerPtr;
  std::shared_ptr<llvm::JITSymbolResolver> SymbolResolverPtr;

  ObjectLayer_t ObjectLayer;
  CompileLayer_t CompileLayer;
};
