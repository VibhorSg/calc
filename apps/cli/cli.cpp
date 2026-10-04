#include "cli.h"

#include <istream>
#include <ostream>
#include <string>

#include "calc/error.h"
#include "calc/evaluator.h"
#include "calc/format.h"
#include "calc/version.h"

namespace calc::cli {

namespace {

constexpr const char* kUsage =
    "Usage: calc [EXPRESSION]\n"
    "\n"
    "Evaluates EXPRESSION and prints the result. With no EXPRESSION, starts an\n"
    "interactive session, or reads one expression per line from standard input\n"
    "when it is not a terminal.\n"
    "\n"
    "Options:\n"
    "  -h, --help     show this help and exit\n"
    "  -v, --version  show the version and exit\n";

constexpr const char* kReplHelp =
    "Enter an expression, e.g. 2 * (3 + 4) or x = sqrt(16).\n"
    "  Operators:  + - * / % ^  (^ is power, right-associative)\n"
    "  Functions:  sin cos tan sqrt log ln abs pow min max\n"
    "  Constants:  pi e      Last result: ans\n"
    "  Commands:   help, quit, exit (or Ctrl-D)\n";

std::string trim(const std::string& s) {
  const auto first = s.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    return "";
  }
  const auto last = s.find_last_not_of(" \t\r\n");
  return s.substr(first, last - first + 1);
}

// Evaluates one line and prints the result or error. Returns false on error.
bool eval_line(const std::string& line, Context& context, std::ostream& out, std::ostream& err) {
  try {
    out << format_number(evaluate(line, context)) << '\n';
    return true;
  } catch (const CalcError& e) {
    err << format_error(line, e) << '\n';
    return false;
  }
}

int run_repl(std::istream& in, std::ostream& out, std::ostream& err) {
  Context context;
  out << "calc " << kVersion << " - type 'help' for help, 'quit' to exit\n";
  std::string line;
  while (true) {
    out << "> " << std::flush;
    if (!std::getline(in, line)) {
      out << '\n';
      return 0;
    }
    const std::string input = trim(line);
    if (input.empty()) {
      continue;
    }
    if (input == "quit" || input == "exit") {
      return 0;
    }
    if (input == "help") {
      out << kReplHelp;
      continue;
    }
    eval_line(input, context, out, err);
  }
}

int run_stream(std::istream& in, std::ostream& out, std::ostream& err) {
  Context context;
  bool ok = true;
  std::string line;
  while (std::getline(in, line)) {
    const std::string input = trim(line);
    if (input.empty()) {
      continue;
    }
    ok = eval_line(input, context, out, err) && ok;
  }
  return ok ? 0 : 1;
}

}  // namespace

int run(const std::vector<std::string>& args, std::istream& in, std::ostream& out,
        std::ostream& err, bool interactive) {
  if (args.empty()) {
    return interactive ? run_repl(in, out, err) : run_stream(in, out, err);
  }
  if (args.size() == 1 && (args[0] == "-h" || args[0] == "--help")) {
    out << kUsage;
    return 0;
  }
  if (args.size() == 1 && (args[0] == "-v" || args[0] == "--version")) {
    out << "calc " << kVersion << '\n';
    return 0;
  }
  if (args.size() > 1) {
    err << "calc: expected one expression; quote it, e.g. calc \"2 * (3 + 4)\"\n" << kUsage;
    return 2;
  }
  Context context;
  return eval_line(trim(args[0]), context, out, err) ? 0 : 1;
}

}  // namespace calc::cli
