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

#include <functional>
#include <memory>

#include "JitFromScratch.h"

#define DEBUG_TYPE "jitfromscratch"

using namespace llvm;

Expected<std::string> codegenIR(Module &module, unsigned items) {
  LLVMContext &ctx = module.getContext();
  IRBuilder<> B(ctx);

  auto name = "abssub";
  auto intTy = Type::getInt32Ty(ctx);
  auto signature = FunctionType::get(intTy, {intTy, intTy}, false);
  auto linkage = Function::ExternalLinkage;

  auto fn = Function::Create(signature, linkage, name, module);

  B.SetInsertPoint(BasicBlock::Create(ctx, "entry", fn));
  {
    Argument *argX = fn->arg_begin();
    Argument *argY = fn->arg_begin() + 1;
    argX->setName("x");
    argY->setName("y");
    Value *difference = B.CreateSub(argX, argY, "dist");

    auto absSig = FunctionType::get(intTy, {intTy}, false);
    FunctionCallee absFunction = module.getOrInsertFunction("abs", absSig);
    Value *absDifference = B.CreateCall(absFunction, {difference});

    B.CreateRet(absDifference);
  }

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

// Also called from JITed code; make sure it's available.
extern "C" int abs(int);

// Temporary global variable to replace below function step-by-step.
std::function<int(int, int)> abssub;

// This function will be replaced by a runtime-time compiled version.
template <size_t sizeOfArray>
int *integerDistances(const int (&x)[sizeOfArray], int *y) {
  unsigned items = arrayElements(x);
  int *results = customIntAllocator(items);

  for (int i = 0; i < items; i++) {
    results[i] = abssub(x[i], y[i]);
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

  JitFromScratch Jit(ExitOnErr);

  LLVM_DEBUG(dbgs() << "JITing for host target: "
                    << Jit.getTargetTriple().normalize() << "\n\n");

  auto C = std::make_unique<LLVMContext>();
  auto M = std::make_unique<Module>("JitFromScratch", *C);
  M->setDataLayout(Jit.getDataLayout());

  std::string JitedFnName = ExitOnErr(codegenIR(*M, arrayElements(x)));
  ExitOnErr(Jit.submitModule(std::move(M), std::move(C)));

  // Request function; this compiles to machine code and links.
  abssub = ExitOnErr(Jit.getFunction<int(int, int)>(JitedFnName));

  int *z = integerDistances(x, y);

  outs() << format("Integer Distances: %d, %d, %d\n\n", z[0], z[1], z[2]);
  outs().flush();

  return 0;
}
