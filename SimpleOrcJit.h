#pragma once

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/NullResolver.h>
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>

using namespace llvm;
using namespace llvm::orc;

class SimpleOrcJit {
  using ModulePtr_t = std::unique_ptr<llvm::Module>;

  using ObjectLayer_t = ObjectLinkingLayer<>;
  using CompileLayer_t = IRCompileLayer<ObjectLayer_t>;

public:
  SimpleOrcJit(TargetMachine &targetMachine)
      : DataLayout(targetMachine.createDataLayout()),
        CompileLayer(ObjectLayer, SimpleCompiler(targetMachine)) {}

  void submitModule(ModulePtr_t module) {
#ifdef DEBUG_DUMP
    outs() << "Submit LLVM module:\n\n";
    outs() << *module.get() << "\n\n";
#endif

    CompileLayer.addModuleSet(singletonSet(std::move(module)),
                              std::make_unique<SectionMemoryManager>(),
                              std::make_unique<NullResolver>());
  }

private:
  DataLayout DataLayout;
  ObjectLayer_t ObjectLayer;
  CompileLayer_t CompileLayer;

  template <typename T> static std::vector<T> singletonSet(T t) {
    std::vector<T> vec;
    vec.push_back(std::move(t));
    return vec;
  }
};
