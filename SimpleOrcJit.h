#pragma once

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/NullResolver.h>
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>
#include <llvm/IR/Mangler.h>
#include <llvm/Support/Debug.h>

#include <functional>
#include <memory>
#include <vector>

#define DEBUG_TYPE "jitfromscratch"

class SimpleOrcJit {
  using ModulePtr_t = std::unique_ptr<llvm::Module>;
  using MemoryManagerPtr_t = std::shared_ptr<llvm::RuntimeDyld::MemoryManager>;
  using SymbolResolverPtr_t = std::shared_ptr<llvm::JITSymbolResolver>;

  using ObjectLayer_t = llvm::orc::ObjectLinkingLayer<>;
  using CompileLayer_t = llvm::orc::IRCompileLayer<ObjectLayer_t>;

public:
  SimpleOrcJit(llvm::TargetMachine &targetMachine)
      : DL(targetMachine.createDataLayout()),
        MemoryManagerPtr(std::make_shared<llvm::SectionMemoryManager>()),
        SymbolResolverPtr(std::make_shared<llvm::orc::NullResolver>()),
        CompileLayer(ObjectLayer, llvm::orc::SimpleCompiler(targetMachine)) {}

  void submitModule(ModulePtr_t module) {
    DEBUG({
      llvm::dbgs() << "Submit LLVM module:\n\n";
      llvm::dbgs() << *module.get() << "\n\n";
    });

    CompileLayer.addModuleSet(singletonSet(std::move(module)), MemoryManagerPtr,
                              SymbolResolverPtr);
  }

  template <class Signature_t>
  std::function<Signature_t> getFunction(std::string unmangledName) {
    auto jitSymbol = CompileLayer.findSymbol(mangle(unmangledName), false);
    auto functionAddr = jitSymbol.getAddress();

    return reinterpret_cast<Signature_t *>(functionAddr);
  }

private:
  llvm::DataLayout DL;
  MemoryManagerPtr_t MemoryManagerPtr;
  SymbolResolverPtr_t SymbolResolverPtr;

  ObjectLayer_t ObjectLayer;
  CompileLayer_t CompileLayer;

  // System name mangler: may prepend '_' on OSX or '\x1' on Windows
  std::string mangle(std::string name) {
    std::string buffer;
    llvm::raw_string_ostream ostream(buffer);
    llvm::Mangler::getNameWithPrefix(ostream, std::move(name), DL);
    return ostream.str();
  }

  template <typename T> static std::vector<T> singletonSet(T t) {
    std::vector<T> vec;
    vec.push_back(std::move(t));
    return vec;
  }
};
