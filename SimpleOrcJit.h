#pragma once

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/LambdaResolver.h>
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/IR/Mangler.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/DynamicLibrary.h>

#include <functional>
#include <memory>
#include <vector>

#define DEBUG_TYPE "jitfromscratch"

#if _WIN32
#define DECL_JIT_ACCESS_C extern "C" __declspec(dllexport)
#else
#define DECL_JIT_ACCESS_C extern "C"
#endif

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
        SymbolResolverPtr(llvm::orc::createLambdaResolver(
            [&](std::string name) { return findSymbolInJITedCode(name); },
            [&](std::string name) { return findSymbolInHostProcess(name); })),
        CompileLayer(ObjectLayer, llvm::orc::SimpleCompiler(targetMachine)) {
    // Load own executable as dynamic library.
    // Required for RTDyldMemoryManager::getSymbolAddressInProcess().
    llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
  }

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
    auto jitSymbol = findSymbolInJITedCode(mangle(unmangledName));
    auto functionAddr = jitSymbol.getAddress();

    return reinterpret_cast<Signature_t *>(functionAddr);
  }

private:
  llvm::DataLayout DL;
  MemoryManagerPtr_t MemoryManagerPtr;
  SymbolResolverPtr_t SymbolResolverPtr;

  ObjectLayer_t ObjectLayer;
  CompileLayer_t CompileLayer;

  llvm::JITSymbol findSymbolInJITedCode(std::string mangledName) {
    constexpr bool exportedSymbolsOnly = false;
    return CompileLayer.findSymbol(mangledName, exportedSymbolsOnly);
  }

  llvm::JITSymbol findSymbolInHostProcess(std::string mangledName) {
    // Lookup function address in the host symbol table.
    if (llvm::JITTargetAddress addr =
            llvm::RTDyldMemoryManager::getSymbolAddressInProcess(mangledName))
      return llvm::JITSymbol(addr, llvm::JITSymbolFlags::Exported);

    return nullptr;
  }

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
