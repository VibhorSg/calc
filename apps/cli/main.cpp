#include <iostream>
#include <string>
#include <vector>

#include "cli.h"

#if defined(_WIN32)
#include <io.h>
#define CALC_ISATTY _isatty
#define CALC_FILENO _fileno
#else
#include <unistd.h>
#define CALC_ISATTY isatty
#define CALC_FILENO fileno
#endif

int main(int argc, char** argv) {
  const std::vector<std::string> args(argv + 1, argv + argc);
  const bool interactive = CALC_ISATTY(CALC_FILENO(stdin)) != 0;
  return calc::cli::run(args, std::cin, std::cout, std::cerr, interactive);
}
