#pragma once

#include <llvm/ADT/Optional.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ExecutionEngine/ObjectCache.h>

#include <memory>
#include <string>

// Simple name-based lookup, based on lli's implementation
class SimpleObjectCache : public llvm::ObjectCache {
public:
  SimpleObjectCache(std::string Dir);
  ~SimpleObjectCache() = default;

  void setCacheModuleName(llvm::Module &M) const;

  llvm::Expected<llvm::Optional<std::unique_ptr<llvm::MemoryBuffer>>>
  getCachedObject(const llvm::Module &M) const;

protected:
  std::unique_ptr<llvm::MemoryBuffer> getObject(const llvm::Module *M) override;
  void notifyObjectCompiled(const llvm::Module *M,
                            llvm::MemoryBufferRef Obj) override;

private:
  bool Enabled;
  std::string CacheDir;

  static std::string endWithSeparator(std::string Path);
  llvm::Optional<std::string> getCacheFileName(llvm::StringRef ModID) const;
};
