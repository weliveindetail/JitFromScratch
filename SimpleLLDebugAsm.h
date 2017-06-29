#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

#include "AsmParser/LLParser.h"
#include "SimpleLLDebugInfo.h"

namespace llvm {

class SimpleLLDebugAsm {

public:
  using DebugFileDeleter_t = std::function<void()>;
  using ModulePtr_t = std::unique_ptr<llvm::Module>;

  SimpleLLDebugAsm() = default;

  static std::tuple<ModulePtr_t, DebugFileDeleter_t>
  Run(ModulePtr_t sourceMod) {
    using namespace llvm;

    std::string code;
    {
      raw_string_ostream codeStream(code);
      sourceMod->print(codeStream, nullptr);
    }

    MemoryBufferRef codeBufferRef(code, sourceMod->getName());
    std::unique_ptr<MemoryBuffer> codeBuffer = MemoryBuffer::getMemBuffer(code);

    auto targetMod = std::make_unique<Module>(
        codeBufferRef.getBufferIdentifier(), sourceMod->getContext());

    targetMod->setDataLayout(sourceMod->getDataLayout());
    targetMod->setTargetTriple(sourceMod->getTargetTriple());

    int fd;
    SmallString<128> filePathName;

    if (auto ec =
            sys::fs::createTemporaryFile("dbas", "ll", fd, filePathName)) {
      errs() << ec.message();
      return std::make_tuple(std::move(sourceMod), []() {});
    }

    DebugFileDeleter_t deleter = [filePathName]() {
      if (auto ec = sys::fs::remove(filePathName))
        errs() << ec.message();
    };

    size_t pathEnd = filePathName.find_last_of(sys::path::get_separator());
    StringRef path = filePathName.substr(0, pathEnd);
    StringRef file = filePathName.substr(pathEnd + 1);

    auto debugInfo =
        std::make_unique<SimpleLLDebugInfo>(targetMod.get(), file, path);

    SMDiagnostic err;
    SourceMgr sourceMgr;
    sourceMgr.AddNewSourceBuffer(MemoryBuffer::getMemBuffer(codeBufferRef),
                                 SMLoc());

    if (LLParser(codeBufferRef.getBuffer(), sourceMgr, err, targetMod.get(),
                 std::move(debugInfo))
            .Run()) {
      err.print("JitFromScratch", errs());
      return std::make_tuple(std::move(sourceMod), []() {});
    }

    std::error_code ec;
    raw_fd_ostream o(filePathName, ec, sys::fs::F_RW);
    targetMod->print(o, nullptr);

    return std::make_tuple(std::move(targetMod), std::move(deleter));
  }
};
}

/*
static std::unique_ptr<llvm::Module>
addIrDebugInfo(std::unique_ptr<llvm::Module> sourceModule,
               llvm::LLVMContext &context, llvm::SMDiagnostic &err) {
  llvm::SourceMgr SM;

  std::string target;
  llvm::raw_string_ostream asmString(target);
  asmString << *sourceModule;
  asmString.flush();

  // llvm::outs() << "Add debug info to module:\n\n";
  // llvm::outs() << *sourceModule << "\n\n";

  llvm::MemoryBufferRef F(target, sourceModule->getName());
  std::unique_ptr<llvm::MemoryBuffer> Buf = llvm::MemoryBuffer::getMemBuffer(F);
  SM.AddNewSourceBuffer(std::move(Buf), llvm::SMLoc());

  std::unique_ptr<llvm::Module> targetModule =
      llvm::make_unique<llvm::Module>(F.getBufferIdentifier(), context);

  targetModule->setDataLayout(sourceModule->getDataLayout());

  if (llvm::LLParser(F.getBuffer(), SM, err, targetModule.get()).Run())
    return nullptr;

  if (!module)
    err.print("JitFromScratch", errs());
  return targetModule;
}


std::tuple < llvm::SmallString<128>,
    std::function<void()> CreateTemporaryLLSourceCodeFile() {
  int fd;
  llvm::SmallString<128> filePathName;

  if (auto ec =
          llvm::sys::fs::createTemporaryFile("dbas", "ll", fd, filePathName)) {
    llvm::errs() << ec.message();
  }

  std::function<void()> deleter = [filePathName]() {
    if (auto ec = llvm::sys::fs::remove(filePathName))
      llvm::errs() << ec.message();
  };
}
*/
