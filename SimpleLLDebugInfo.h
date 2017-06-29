#pragma once

#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/Module.h>

namespace llvm {

class SimpleLLDebugInfo {
public:
  SimpleLLDebugInfo(Module *module, StringRef file, StringRef directory)
      : Module(module), Builder(*module) {
    Module->addModuleFlag(Module::Error, "Debug Info Version",
                          (uint32_t)DEBUG_METADATA_VERSION);
    Module->addModuleFlag(Module::Error, "Dwarf Version", 4);

    SourceFile = Builder.createFile(file, directory);
    CompileUnit = Builder.createCompileUnit(dwarf::DW_LANG_lo_user, SourceFile,
                                            "llvm-dbas", false, "", 0);
  }

  DISubprogram *addFunction(Function *F) {
    DITypeRefArray a;
    DISubroutineType *ST = Builder.createSubroutineType(a);
    DISubprogram *SP =
        Builder.createFunction(SourceFile, F->getName(), F->getName(),
                               SourceFile, 0, ST, true, true, 0);
    F->setSubprogram(SP);
    return SP;
  }

  void addInstruction(Instruction *I, DISubprogram *SP, unsigned int line) {
    DebugLoc loc = DebugLoc::get(line, 0, SP);
    I->setDebugLoc(loc);
  }

  void finalize() { Builder.finalize(); }

private:
  Module *Module;
  DIBuilder Builder;
  DICompileUnit *CompileUnit;
  DIFile *SourceFile;
};
}
