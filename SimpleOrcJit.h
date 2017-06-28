#pragma once

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/LambdaResolver.h>
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/IR/Mangler.h>
#include <llvm/Support/DynamicLibrary.h>

using namespace llvm;
using namespace llvm::orc;

class SimpleOrcJit {
  using ModulePtr_t = std::unique_ptr<llvm::Module>;

  using ObjectLayer_t = ObjectLinkingLayer<>;
  using CompileLayer_t = IRCompileLayer<ObjectLayer_t>;

public:
  SimpleOrcJit(TargetMachine &targetMachine)
      : DataLayout(targetMachine.createDataLayout()),
        CompileLayer(ObjectLayer, SimpleCompiler(targetMachine)) {
    sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
  }

  void submitModule(ModulePtr_t module) {
#ifdef DEBUG_DUMP
    outs() << "Submit LLVM module:\n\n";
    outs() << *module.get() << "\n\n";
#endif

    auto lambdaResolver = createLambdaResolver(
        [&](std::string name) {
          if (auto Sym = findMangledSymbol(name))
            return JITSymbol(Sym.getAddress(), Sym.getFlags());

          return JITSymbol(nullptr);
        },
        [](std::string) { return nullptr; });

    CompileLayer.addModuleSet(singletonSet(std::move(module)),
                              std::make_unique<SectionMemoryManager>(),
                              std::move(lambdaResolver));
  }

  template <class Signature_t>
  std::function<Signature_t> getFunction(std::string unmangledName) {
    auto jitSymbol = CompileLayer.findSymbol(mangle(unmangledName), false);
    auto functionAddr = jitSymbol.getAddress();

    return (Signature_t *)functionAddr;
  }

private:
  DataLayout DataLayout;
  ObjectLayer_t ObjectLayer;
  CompileLayer_t CompileLayer;

  JITSymbol findMangledSymbol(std::string name) {
    // find symbols in host process
    if (auto SymAddr = RTDyldMemoryManager::getSymbolAddressInProcess(name))
      return JITSymbol(SymAddr, JITSymbolFlags::Exported);

    return nullptr;
  }

  std::string mangle(std::string name) {
    std::string mangledName;
    {
      raw_string_ostream ostream(mangledName);
      Mangler::getNameWithPrefix(ostream, std::move(name), DataLayout);
    }
    return mangledName;
  }

  template <typename T> static std::vector<T> singletonSet(T t) {
    std::vector<T> vec;
    vec.push_back(std::move(t));
    return vec;
  }
};
