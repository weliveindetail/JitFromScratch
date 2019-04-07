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

#include <functional>
#include <memory>

#include "JitFromScratch.h"

#define DEBUG_TYPE "jitfromscratch"

using namespace llvm;

static cl::opt<char>
    OptLevel("O",
        cl::desc("Optimization level. [-O0, -O1, -O2, or -O3] "
                  "(default = '-O2')"),
        cl::Prefix, cl::ZeroOrMore, cl::init(' '));

static cl::opt<std::string>
    CacheDir("cache-dir",
        cl::desc("Pass a directory name to enable object file cache"),
        cl::value_desc("directory"), cl::init(""));

Expected<unsigned> getOptLevel() {
  switch (OptLevel) {
    case '0': return 0;
    case '1': return 1;
    case ' ':
    case '2': return 2;
    case '3': return 3;
    default:
      return createStringError(inconvertibleErrorCode(),
                               "Invalid optimization level: -O%c",
                               (char)OptLevel);
  }
}

Expected<std::string> codegenIR(Module &module, unsigned items) {
  LLVMContext &ctx = module.getContext();
  IRBuilder<> B(ctx);

  auto name = "integerDistances";
  auto intTy = Type::getInt32Ty(ctx);
  auto intPtrTy = intTy->getPointerTo();
  auto signature = FunctionType::get(intPtrTy, {intPtrTy, intPtrTy}, false);
  auto linkage = Function::ExternalLinkage;

  auto fn = Function::Create(signature, linkage, name, module);

  B.SetInsertPoint(BasicBlock::Create(ctx, "entry", fn));
  {
    Argument *argX = fn->arg_begin();
    Argument *argY = fn->arg_begin() + 1;
    argX->setName("xs_ptr");
    argY->setName("ys_ptr");

    auto absSig = FunctionType::get(intTy, {intTy}, false);
    Value *absFunction = module.getOrInsertFunction("abs", absSig);

    auto allocSig = FunctionType::get(intPtrTy, {intTy}, false);
    Value *allocFunction =
        module.getOrInsertFunction("customIntAllocator", allocSig);

    Value *rs_count = ConstantInt::get(intTy, items);
    Value *rs_ptr = B.CreateCall(allocFunction, {rs_count}, "rs_ptr");

    for (unsigned int i = 0; i < items; i++) {
      Value *xi_ptr = B.CreateConstInBoundsGEP1_32(intTy, argX, i, "x_ptr");
      Value *yi_ptr = B.CreateConstInBoundsGEP1_32(intTy, argY, i, "y_ptr");

      Value *xi = B.CreateLoad(xi_ptr, "x");
      Value *yi = B.CreateLoad(yi_ptr, "y");
      Value *difference = B.CreateSub(xi, yi, "diff");
      Value *absDifference = B.CreateCall(absFunction, {difference}, "dist");

      Value *ri_ptr = B.CreateConstInBoundsGEP1_32(intTy, rs_ptr, i, "ri_ptr");
      B.CreateStore(absDifference, ri_ptr);
    }

    B.CreateRet(rs_ptr);
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
extern "C" int *customIntAllocator(unsigned items) {
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

  LLVM_DEBUG(dbgs() << "JITing for host target: "
                    << TM->getTargetTriple().normalize() << "\n\n");

  auto C = std::make_unique<LLVMContext>();
  auto M = std::make_unique<Module>("JitFromScratch", *C);

  std::string JitedFnName = ExitOnErr(codegenIR(*M, arrayElements(x)));
  unsigned OptLevel = ExitOnErr(getOptLevel());
  bool AddToCache = !CacheDir.empty();

  JitFromScratch Jit(std::move(TM), CacheDir);
  ExitOnErr(Jit.submitModule(std::move(M), std::move(C), OptLevel, AddToCache));

  // Request function; this compiles to machine code and links.
  auto integerDistances =
      ExitOnErr(Jit.getFunction<int *(int *, int *)>(JitedFnName));

  int *z = integerDistances(x, y);

  outs() << format("Integer Distances: %d, %d, %d\n\n", z[0], z[1], z[2]);
  outs().flush();

  return 0;
}
