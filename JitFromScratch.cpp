#include "llvm/Support/raw_ostream.h"

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
int *integerDistances(const int (&x)[sizeOfArray], int *y) {
  unsigned items = arrayElements(x);
  int *results = customIntAllocator(items);

  for (int i = 0; i < items; i++) {
    results[i] = abs(x[i] - y[i]);
  }

  return results;
}

int main() {
  int x[]{0, 1, 2};
  int y[]{3, 1, -1};
  int *z = integerDistances(x, y);

  llvm::outs() << "Integer Distances: ";
  llvm::outs() << z[0] << ", " << z[1] << ", " << z[2] << "\n\n";
  llvm::outs().flush();

  return 0;
}
