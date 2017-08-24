#pragma once

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/IRTransformLayer.h>
#include <llvm/ExecutionEngine/Orc/LambdaResolver.h>
#include <llvm/ExecutionEngine/Orc/OrcError.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/Mangler.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Transforms/Scalar.h>

#include <functional>
#include <memory>
#include <vector>

#define DEBUG_TYPE "jitfromscratch"

class SimpleOrcJit {
  using ModulePtr_t = std::unique_ptr<llvm::Module>;
  using IRCompiler_t = llvm::orc::SimpleCompiler;

  using ModuleSharedPtr_t = std::shared_ptr<llvm::Module>;
  using Optimize_f = std::function<ModuleSharedPtr_t(ModuleSharedPtr_t)>;

  using ObjectLayer_t = llvm::orc::RTDyldObjectLinkingLayer;
  using CompileLayer_t = llvm::orc::IRCompileLayer<ObjectLayer_t, IRCompiler_t>;
  using OptimizeLayer_t =
      llvm::orc::IRTransformLayer<CompileLayer_t, Optimize_f>;

public:
  SimpleOrcJit(llvm::TargetMachine &targetMachine)
      : DL(targetMachine.createDataLayout()),
        MemoryManagerPtr(std::make_shared<llvm::SectionMemoryManager>()),
        SymbolResolverPtr(llvm::orc::createLambdaResolver(
            [&](std::string name) { return findSymbolInJITedCode(name); },
            [&](std::string name) { return findSymbolInHostProcess(name); })),
        ObjectLayer([this]() { return MemoryManagerPtr; }),
        CompileLayer(ObjectLayer, IRCompiler_t(targetMachine)),
        OptimizeLayer(CompileLayer, [this](ModuleSharedPtr_t module) {
          return optimizeModule(std::move(module));
        }) {
    // Load own executable as dynamic library.
    // Required for RTDyldMemoryManager::getSymbolAddressInProcess().
    llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
  }

  void submitModule(ModulePtr_t module) {
    DEBUG({
      llvm::dbgs() << "Submit LLVM module:\n\n";
      llvm::dbgs() << *module.get() << "\n\n";
    });

    // Commit module for compilation to machine code. Actual compilation
    // happens on demand as soon as one of it's symbols is accessed. None of
    // the layers used here issue Errors from this call.
    llvm::cantFail(
        OptimizeLayer.addModule(std::move(module), SymbolResolverPtr));
  }

  template <class Signature_t>
  llvm::Expected<std::function<Signature_t>> getFunction(std::string name) {
    using namespace llvm;

    // Find symbol name in committed modules.
    std::string mangledName = mangle(std::move(name));
    JITSymbol sym = findSymbolInJITedCode(mangledName);
    if (!sym)
      return make_error<orc::JITSymbolNotFound>(mangledName);

    // Access symbol address.
    // Invokes compilation for the respective module if not compiled yet.
    Expected<JITTargetAddress> addr = sym.getAddress();
    if (!addr)
      return addr.takeError();

    auto typedFunctionPtr = reinterpret_cast<Signature_t *>(*addr);
    return std::function<Signature_t>(typedFunctionPtr);
  }

private:
  llvm::DataLayout DL;
  std::shared_ptr<llvm::RTDyldMemoryManager> MemoryManagerPtr;
  std::shared_ptr<llvm::JITSymbolResolver> SymbolResolverPtr;

  ObjectLayer_t ObjectLayer;
  CompileLayer_t CompileLayer;
  OptimizeLayer_t OptimizeLayer;

  ModuleSharedPtr_t optimizeModule(ModuleSharedPtr_t module) {
    using namespace llvm;

    PassManagerBuilder PMBuilder;
    PMBuilder.LoopVectorize = true;
    PMBuilder.SLPVectorize = true;
    PMBuilder.VerifyInput = true;
    PMBuilder.VerifyOutput = true;

    legacy::FunctionPassManager perFunctionPasses(module.get());
    PMBuilder.populateFunctionPassManager(perFunctionPasses);

    perFunctionPasses.doInitialization();

    for (Function &function : *module)
      perFunctionPasses.run(function);

    perFunctionPasses.doFinalization();

    legacy::PassManager perModulePasses;
    PMBuilder.populateModulePassManager(perModulePasses);
    perModulePasses.run(*module);

    DEBUG({
      outs() << "Optimized module:\n\n";
      outs() << *module.get() << "\n\n";
    });

    return module;
  }

  llvm::JITSymbol findSymbolInJITedCode(std::string mangledName) {
    constexpr bool exportedSymbolsOnly = false;
    return CompileLayer.findSymbol(mangledName, exportedSymbolsOnly);
  }

  llvm::JITSymbol findSymbolInHostProcess(std::string mangledName) {
    // Hack: Provide function pointer for dedicated externals.
    if (mangledName == mangle("customIntAllocator"))
      return llvm::JITSymbol(llvm::JITTargetAddress(&customIntAllocator),
                             llvm::JITSymbolFlags::Exported);

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
};
