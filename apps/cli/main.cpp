#include <iostream>

#include "calc/version.h"

int main() {
  std::cout << "calc " << calc::kVersion << '\n';
  return 0;
}
