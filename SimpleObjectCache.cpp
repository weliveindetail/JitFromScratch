#include "SimpleObjectCache.h"

#include <llvm/ADT/None.h>
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/Twine.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/raw_ostream.h>

#define DEBUG_TYPE "jitfromscratch"

using namespace llvm;

SimpleObjectCache::SimpleObjectCache(std::string Dir)
    : Enabled(!Dir.empty()), CacheDir(endWithSeparator(std::move(Dir))) {
  if (!sys::fs::exists(CacheDir)) {
    LLVM_DEBUG(dbgs() << format("Create new cache directory '%s'\n\n",
                                CacheDir.c_str()));
    std::error_code EC = sys::fs::create_directories(CacheDir);
    if (EC) {
      LLVM_DEBUG(dbgs() << format(
              "Creating new cache directory '%s' failed with "
              "error code %d; Caching disabled\n\n",
              CacheDir.c_str(), EC.value()));
      Enabled = false;
    }
  }
}

std::string SimpleObjectCache::endWithSeparator(std::string Path) {
  return Path.back() == '/' ? Path : Path + "/";
}

// Implements llvm::ObjectCache::notifyObjectCompiled, called from CompileLayer
void SimpleObjectCache::notifyObjectCompiled(const Module *M,
                                             MemoryBufferRef Obj) {
  assert(M && "Caching requires module");

  auto R = getCacheFileName(M->getModuleIdentifier());
  if (!R.hasValue())
    return;

  std::string F = std::move(R.getValue());
  if (auto EC = sys::fs::create_directories(sys::path::parent_path(F))) {
    LLVM_DEBUG(dbgs() << format(
                   "Writing cached object '%s' failed with error code %d\n\n",
                   F.c_str(), EC.value()));
    return;
  }

  std::error_code EC;
  raw_fd_ostream OS(F, EC, sys::fs::F_None);
  if (EC) {
    LLVM_DEBUG(dbgs() << format(
                   "Writing cached object '%s' failed with error code %d\n\n",
                   F.c_str(), EC.value()));
    return;
  }

  LLVM_DEBUG(dbgs() << format("Write cached object '%s'\n\n", F.c_str()));

  OS.write(Obj.getBufferStart(), Obj.getBufferSize());
  OS.close();
}

// Implements llvm::ObjectCache::getObject, called from CompileLayer
std::unique_ptr<MemoryBuffer> SimpleObjectCache::getObject(const Module *M) {
  assert(M && "Lookup requires module");

  auto R = getCachedObject(*M);
  if (!R) {
    logAllUnhandledErrors(R.takeError(), dbgs(), "SimpleObjectCache: ");
    return nullptr; // Error
  }

  if (!R->hasValue())
    return nullptr; // No cache entry

  return std::forward<std::unique_ptr<MemoryBuffer>>(R->getValue());
}

Expected<Optional<std::unique_ptr<MemoryBuffer>>>
SimpleObjectCache::getCachedObject(const Module &M) const {
  auto R = getCacheFileName(M.getModuleIdentifier());
  if (!R.hasValue())
    return None;

  std::string F = std::move(R.getValue());
  if (!sys::fs::exists(F))
    return None;

  auto B = MemoryBuffer::getFile(F, -1, false);
  if (!B)
    return createStringError(
        B.getError(),
        "Reading cached object '%s' failed with error code %d\n\n", F.c_str(),
        B.getError().value());

  LLVM_DEBUG(dbgs() << format("Read cached object '%s'\n\n", F.c_str()));
  return std::forward<std::unique_ptr<MemoryBuffer>>(*B);
}

void SimpleObjectCache::setCacheModuleName(Module &M) const {
  if (Enabled && !M.getName().startswith("file:"))
    M.setModuleIdentifier("file:" + M.getModuleIdentifier() + ".o");
}

Optional<std::string>
SimpleObjectCache::getCacheFileName(StringRef ModID) const {
  if (!Enabled)
    return None;

  StringRef Prefix = "file:";
  if (!ModID.startswith(Prefix))
    return None;

  std::string Name = Twine(CacheDir + ModID.substr(Prefix.size())).str();
  size_t DotPos = Name.rfind('.');
  if (DotPos != std::string::npos)
    Name.replace(DotPos, Name.size() - DotPos, ".o");

  return Name;
}
