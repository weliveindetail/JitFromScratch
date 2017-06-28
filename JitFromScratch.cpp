#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/Signals.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include "SimpleOrcJit.h"

llvm::Expected<std::string> codegenIR(Module *module) {
  using namespace llvm;

  LLVMContext &ctx = module->getContext();
  IRBuilder<> Builder(ctx);

  auto name = "substract";
  auto returnTy = Type::getInt32Ty(ctx);
  auto argTy = Type::getInt32Ty(ctx);
  auto signature = FunctionType::get(returnTy, {argTy, argTy}, false);
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
    Builder.CreateRet(difference);
  }

  std::string error;
  raw_string_ostream es(error);

  if (verifyFunction(*fn, &es))
    return make_error<StringError>(
        Twine("Function verification failed:\n", es.str()), std::error_code());

  if (verifyModule(*module, &es))
    return make_error<StringError>(
        Twine("Module verification failed:\n", es.str()), std::error_code());

  return name;
}

template <typename T, size_t sizeOfArray>
constexpr int arrayElements(T (&)[sizeOfArray]) {
  return sizeOfArray;
}

int *customIntAllocator(int items) {
  static int memory[100];
  static int allocIdx = 0;

  if (allocIdx + items > arrayElements(memory))
    exit(-1);

  int *block = memory + allocIdx;
  allocIdx += items;

  return block;
}

// goal for jit-basics:
// replace this function with a runtime-time compiled version
template <size_t sizeOfArray>
int *integerDistances(const int (&x)[sizeOfArray], int *y,
                      std::function<int(int, int)> jittedFn) {
  int items = arrayElements(x);
  int *results = customIntAllocator(items);

  for (int i = 0; i < items; i++) {
    results[i] = abs(jittedFn(x[i], y[i]));
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

  auto *targetMachine = EngineBuilder().selectTarget();
  auto jit = std::make_unique<SimpleOrcJit>(*targetMachine);

  LLVMContext context;
  auto module = std::make_unique<Module>("JitFromScratch", context);
  module->setDataLayout(targetMachine->createDataLayout());

  Expected<std::string> jittedFnName = codegenIR(module.get());
  if (!jittedFnName)
    outs() << toString(jittedFnName.takeError());

  jit->submitModule(std::move(module));
  auto jittedFnPtr = jit->getFunction<int(int, int)>(*jittedFnName);

  int x[]{0, 1, 2};
  int y[]{3, 1, -1};
  int *z = integerDistances(x, y, jittedFnPtr);

  outs() << "Integer Distances: ";
  outs() << z[0] << ", " << z[1] << ", " << z[2] << "\n\n";
  outs().flush();

  return 0;
}
