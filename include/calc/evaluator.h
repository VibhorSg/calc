#pragma once

#include <map>
#include <optional>
#include <string>
#include <string_view>

#include "calc/ast.h"

namespace calc {

// Session state: user variables and the last result (`ans`). Built-in
// constants (pi, e) are not stored here and cannot be reassigned.
class Context {
 public:
  void set(const std::string& name, double value) { variables_[name] = value; }
  std::optional<double> get(const std::string& name) const;
  const std::map<std::string, double>& variables() const { return variables_; }

 private:
  std::map<std::string, double> variables_;
};

// Evaluates a tree. Assignments write into `context`; `ans` is not updated
// here (see the string overload). Throws CalcError on unknown identifiers,
// wrong arity, division by zero and domain errors (including any non-finite
// result).
double evaluate(const Node& node, Context& context);

// Tokenizes, parses and evaluates `input`, then stores the result in `ans`.
double evaluate(std::string_view input, Context& context);

// True for names that cannot be assigned to: constants and built-in functions.
bool is_reserved_name(std::string_view name);

}  // namespace calc
