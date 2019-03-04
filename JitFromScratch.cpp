#include "JitFromScratch.h"

using namespace llvm;
using namespace llvm::orc;

JitFromScratch::JitFromScratch(std::unique_ptr<TargetMachine> TM, DataLayout DL)
    : LLJIT(std::make_unique<ExecutionSession>(), std::move(TM), DL) {}
