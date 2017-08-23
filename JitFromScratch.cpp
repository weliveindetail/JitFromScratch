#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/Signals.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include "SimpleOrcJit.h"

// Show the error message and exit.
LLVM_ATTRIBUTE_NORETURN static void fatalError(llvm::Error E) {
  llvm::handleAllErrors(std::move(E), [&](const llvm::ErrorInfoBase &EI) {
    llvm::errs() << "Fatal Error: ";
    EI.log(llvm::errs());
    llvm::errs() << "\n";
    llvm::errs().flush();
  });

  exit(1);
}

llvm::Expected<std::string> codegenIR(llvm::Module *module) {
  using namespace llvm;

  LLVMContext &ctx = module->getContext();
  IRBuilder<> Builder(ctx);

  auto name = "abssub";
  auto intTy = Type::getInt32Ty(ctx);
  auto signature = FunctionType::get(intTy, {intTy, intTy}, false);
  auto linkage = Function::ExternalLinkage;

  auto fn = Function::Create(signature, linkage, name, module);
  fn->setName(name); // so the CompileLayer can find it

  Builder.SetInsertPoint(BasicBlock::Create(ctx, "entry", fn));
  {
    auto argIt = fn->arg_begin();
    Argument &argX = *argIt;
    Argument &argY = *(++argIt);
    argX.setName("x");
    argY.setName("y");

    Value *difference = Builder.CreateSub(&argX, &argY, "dist");

    auto absSig = FunctionType::get(intTy, {intTy}, false);
    Value *absFunction = module->getOrInsertFunction("abs", absSig);
    Value *absDifference = Builder.CreateCall(absFunction, {difference});

    Builder.CreateRet(absDifference);
  }

  std::string error;
  raw_string_ostream es(error);

  if (verifyFunction(*fn, &es))
    return make_error<StringError>(
        Twine("Function verification failed:\n", es.str()),
        llvm::inconvertibleErrorCode());

  if (verifyModule(*module, &es))
    return make_error<StringError>(
        Twine("Module verification failed:\n", es.str()),
        llvm::inconvertibleErrorCode());

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
int *integerDistances(const int (&x)[sizeOfArray], int *y,
                      std::function<int(int, int)> jitedFn) {
  unsigned items = arrayElements(x);
  int *results = customIntAllocator(items);

  for (int i = 0; i < items; i++) {
    results[i] = jitedFn(x[i], y[i]);
  }

  return results;
}

int main(int argc, char **argv) {
  using namespace llvm;

  sys::PrintStackTraceOnErrorSignal(argv[0]);
  PrettyStackTraceProgram X(argc, argv);

  atexit(llvm_shutdown);

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  // Parse -debug and -debug-only options.
  cl::ParseCommandLineOptions(argc, argv, "JitFromScratch example project\n");

  int x[]{0, 1, 2};
  int y[]{3, 1, -1};

  auto targetMachine = EngineBuilder().selectTarget();
  auto jit = std::make_unique<SimpleOrcJit>(*targetMachine);

  LLVMContext context;
  auto module = std::make_unique<Module>("JitFromScratch", context);
  module->setDataLayout(targetMachine->createDataLayout());

  // Generate LLVM IR for the function.
  Expected<std::string> jitedFnName = codegenIR(module.get());
  if (!jitedFnName)
    fatalError(jitedFnName.takeError());

  // Compile to machine code and link.
  jit->submitModule(std::move(module));
  auto jitedFn = jit->getFunction<int(int, int)>(*jitedFnName);
  if (!jitedFn)
    fatalError(jitedFn.takeError());

  int *z = integerDistances(x, y, *jitedFn);

  outs() << "Integer Distances: ";
  outs() << z[0] << ", " << z[1] << ", " << z[2] << "\n\n";
  outs().flush();

  return 0;
}
