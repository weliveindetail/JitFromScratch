#pragma once

#include "ClangCC1Driver.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
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
#define DECL_JIT_ACCESS_CPP __declspec(dllexport)
#else
#define DECL_JIT_ACCESS_CPP
#endif

class SimpleOrcJit {
  class NotifyObjectLoaded_t {
    // No templates but shortcuts for typenames to get straight compiler errors
    // for API changes.
    template <class Item_t>
    using UniquePtrs_t = std::vector<std::unique_ptr<Item_t>>;

    using LoadedObjInfoList_t =
        UniquePtrs_t<llvm::RuntimeDyld::LoadedObjectInfo>;

    using ObjList_t =
        UniquePtrs_t<llvm::object::OwningBinary<llvm::object::ObjectFile>>;

    using UnusedObjHandle_t = llvm::orc::ObjectLinkingLayerBase::ObjSetHandleT;

  public:
    NotifyObjectLoaded_t(SimpleOrcJit &jit) : Jit(jit) {}

    // Called by the ObjectLayer for each emitted object-set.
    // Forward notification to GDB JIT interface.
    void operator()(UnusedObjHandle_t, const ObjList_t &objs,
                    const LoadedObjInfoList_t &infos) const {
      for (unsigned i = 0; i < objs.size(); ++i)
        Jit.GdbEventListener->NotifyObjectEmitted(*objs[i]->getBinary(),
                                                  *infos[i]);
    }

  private:
    SimpleOrcJit &Jit;
  };

  using ModulePtr_t = std::unique_ptr<llvm::Module>;
  using MemoryManagerPtr_t = std::shared_ptr<llvm::RuntimeDyld::MemoryManager>;
  using SymbolResolverPtr_t = std::shared_ptr<llvm::JITSymbolResolver>;

  using ObjectLayer_t = llvm::orc::ObjectLinkingLayer<NotifyObjectLoaded_t>;
  using CompileLayer_t = llvm::orc::IRCompileLayer<ObjectLayer_t>;

public:
  SimpleOrcJit(llvm::TargetMachine &targetMachine)
      : DL(targetMachine.createDataLayout()),
        MemoryManagerPtr(std::make_shared<llvm::SectionMemoryManager>()),
        SymbolResolverPtr(llvm::orc::createLambdaResolver(
            [&](std::string name) { return findSymbolInJITedCode(name); },
            [&](std::string name) { return findSymbolInHostProcess(name); })),
        NotifyObjectLoaded(*this), ObjectLayer(NotifyObjectLoaded),
        CompileLayer(ObjectLayer, llvm::orc::SimpleCompiler(targetMachine)) {
    // Load own executable as dynamic library.
    // Required for RTDyldMemoryManager::getSymbolAddressInProcess().
    llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

    // Internally points to a llvm::ManagedStatic.
    // No need to free. "create" is a misleading term here.
    GdbEventListener = llvm::JITEventListener::createGDBRegistrationListener();
  }

  void submitModule(ModulePtr_t module) {
    DEBUG({
      llvm::dbgs() << "Submit LLVM module:\n\n";
      llvm::dbgs() << *module.get() << "\n\n";
    });

    CompileLayer.addModuleSet(singletonSet(std::move(module)), MemoryManagerPtr,
                              SymbolResolverPtr);
  }

  llvm::Expected<std::unique_ptr<llvm::Module>>
  compileModuleFromCpp(std::string cppCode, llvm::LLVMContext &context) {
    return ClangDriver.compileTranslationUnit(cppCode, context);
  }

  template <class Signature_t>
  std::function<Signature_t> getFunction(std::string unmangledName) {
    auto jitSymbol = findSymbolInJITedCode(mangle(unmangledName));
    auto functionAddr = jitSymbol.getAddress();

    return reinterpret_cast<Signature_t *>(functionAddr);
  }

private:
  llvm::DataLayout DL;
  ClangCC1Driver ClangDriver;
  MemoryManagerPtr_t MemoryManagerPtr;
  SymbolResolverPtr_t SymbolResolverPtr;
  NotifyObjectLoaded_t NotifyObjectLoaded;
  llvm::JITEventListener *GdbEventListener;

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
