#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/Format.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>

#include <memory>

#include "JitFromScratch.h"

#define DEBUG_TYPE "jitfromscratch"

using namespace llvm;

Expected<std::string> codegenIR(Module &module, unsigned items) {
  LLVMContext &ctx = module.getContext();
  IRBuilder<> B(ctx);

  auto name = "getZero";
  auto returnTy = Type::getInt32Ty(ctx);
  auto argTy = Type::getInt32Ty(ctx);
  auto signature = FunctionType::get(returnTy, {argTy, argTy}, false);
  auto linkage = Function::ExternalLinkage;

  auto fn = Function::Create(signature, linkage, name, module);

  B.SetInsertPoint(BasicBlock::Create(ctx, "entry", fn));
  B.CreateRet(ConstantInt::get(returnTy, 0));

  std::string buffer;
  raw_string_ostream es(buffer);

  if (verifyFunction(*fn, &es))
    return createStringError(inconvertibleErrorCode(),
                             "Function verification failed: %s",
                             es.str().c_str());

  if (verifyModule(module, &es))
    return createStringError(inconvertibleErrorCode(),
                             "Module verification failed: %s",
                             es.str().c_str());

  return name;
}

// Determine the size of a C array at compile-time.
template <typename T, size_t sizeOfArray>
constexpr unsigned arrayElements(T (&)[sizeOfArray]) {
  return sizeOfArray;
}

// This function will be called from JITed code.
int *customIntAllocator(unsigned items) {
  static int memory[100];
  static unsigned allocIdx = 0;

  if (allocIdx + items > arrayElements(memory))
    exit(-1);

  int *block = memory + allocIdx;
  allocIdx += items;

  return block;
}

// This function will be replaced by a runtime-time compiled version.
template <size_t sizeOfArray>
int *integerDistances(const int (&x)[sizeOfArray], int *y) {
  unsigned items = arrayElements(x);
  int *results = customIntAllocator(items);

  for (int i = 0; i < items; i++) {
    results[i] = abs(x[i] - y[i]);
  }

  return results;
}

int main(int argc, char **argv) {
  InitLLVM X(argc, argv);

  ExitOnError ExitOnErr;
  ExitOnErr.setBanner("JitFromScratch: ");

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  // Parse implicit -debug and -debug-only options.
  cl::ParseCommandLineOptions(argc, argv, "JitFromScratch example project\n");

  int x[]{0, 1, 2};
  int y[]{3, 1, -1};

  std::unique_ptr<TargetMachine> TM(EngineBuilder().selectTarget());
  DataLayout DL = TM->createDataLayout();

  LLVM_DEBUG(dbgs() << "JITing for host target: "
                    << TM->getTargetTriple().normalize() << "\n\n");

  auto C = std::make_unique<LLVMContext>();
  auto M = std::make_unique<Module>("JitFromScratch", *C);
  M->setDataLayout(DL);

  ExitOnErr(codegenIR(*M, arrayElements(x)));

  JitFromScratch Jit(std::move(TM), DL);
  ExitOnErr(Jit.submitModule(std::move(M), std::move(C)));

  int *z = integerDistances(x, y);

  outs() << format("Integer Distances: %d, %d, %d\n\n", z[0], z[1], z[2]);
  outs().flush();

  return 0;
}
