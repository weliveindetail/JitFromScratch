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

// Determine the size of a C array at compile-time.
template <typename T, size_t sizeOfArray>
constexpr unsigned arrayElements(T (&)[sizeOfArray]) {
  return sizeOfArray;
}

// This function will be called from JITed code.
DECL_JIT_ACCESS_CPP int *customIntAllocator(unsigned items) {
  static int memory[100];
  static unsigned allocIdx = 0;

  if (allocIdx + items > arrayElements(memory))
    exit(-1);

  int *block = memory + allocIdx;
  allocIdx += items;

  return block;
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

  // Implementation of the integerDistances function.
  std::string sourceCode =
      "extern \"C\" int abs(int);                                           \n"
      "extern int *customIntAllocator(unsigned items);                      \n"
      "                                                                     \n"
      "extern \"C\" int *integerDistances(int* x, int *y, unsigned items) { \n"
      "  int *results = customIntAllocator(items);                          \n"
      "                                                                     \n"
      "  for (int i = 0; i < items; i++) {                                  \n"
      "    results[i] = abs(x[i] - y[i]);                                   \n"
      "  }                                                                  \n"
      "                                                                     \n"
      "  return results;                                                    \n"
      "}                                                                    \n";

  // Compile C++ to bitcode.
  LLVMContext context;
  auto module = jit->compileModuleFromCpp(sourceCode, context);
  if (!module)
    fatalError(module.takeError());

  // Compile to machine code and link.
  jit->submitModule(std::move(*module));
  auto jitedFn =
      jit->getFunction<int *(int *, int *, unsigned)>("integerDistances");
  if (!jitedFn)
    fatalError(jitedFn.takeError());

  // Invoke JITed function.
  auto integerDistances = *jitedFn;
  int *z = integerDistances(x, y, arrayElements(x));

  outs() << "Integer Distances: ";
  outs() << z[0] << ", " << z[1] << ", " << z[2] << "\n\n";
  outs().flush();

  return 0;
}
