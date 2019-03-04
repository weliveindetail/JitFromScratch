#include "JitFromScratch.h"

using namespace llvm;
using namespace llvm::orc;

JitFromScratch::JitFromScratch(ExitOnError ExitOnErr)
    : LLJIT(ExitOnErr(LLJITBuilder().create())) {}
