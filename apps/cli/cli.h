#pragma once

#include <iosfwd>
#include <string>
#include <vector>

namespace calc::cli {

// Runs the calc command line. `args` excludes the program name.
//   calc "<expr>"   evaluate one expression
//   calc            interactive REPL when `interactive`, otherwise read
//                   expressions line by line from `in`
// Returns the process exit code: 0 on success, 1 if any expression failed,
// 2 on bad usage.
int run(const std::vector<std::string>& args, std::istream& in, std::ostream& out,
        std::ostream& err, bool interactive);

}  // namespace calc::cli
