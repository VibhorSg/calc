#include "calc/evaluator.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include "calc/error.h"
#include "calc/parser.h"
#include "calc/tokenizer.h"

namespace calc {

namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr double kE = 2.71828182845904523536;
constexpr std::size_t kVariadic = std::numeric_limits<std::size_t>::max();

using Args = std::vector<double>;

struct Builtin {
  std::size_t min_args;
  std::size_t max_args;
  std::function<double(const Args&, std::size_t position)> fn;
};

[[noreturn]] void domain_error(const std::string& message, std::size_t position) {
  throw CalcError(CalcError::Kind::Domain, message, position);
}

const std::unordered_map<std::string, Builtin>& builtins() {
  static const std::unordered_map<std::string, Builtin> table = {
      {"sin", {1, 1, [](const Args& a, std::size_t) { return std::sin(a[0]); }}},
      {"cos", {1, 1, [](const Args& a, std::size_t) { return std::cos(a[0]); }}},
      {"tan", {1, 1, [](const Args& a, std::size_t) { return std::tan(a[0]); }}},
      {"abs", {1, 1, [](const Args& a, std::size_t) { return std::fabs(a[0]); }}},
      {"sqrt",
       {1, 1,
        [](const Args& a, std::size_t pos) {
          if (a[0] < 0) {
            domain_error("sqrt of a negative number", pos);
          }
          return std::sqrt(a[0]);
        }}},
      {"ln",
       {1, 1,
        [](const Args& a, std::size_t pos) {
          if (a[0] <= 0) {
            domain_error("ln of a non-positive number", pos);
          }
          return std::log(a[0]);
        }}},
      {"log",
       {1, 1,
        [](const Args& a, std::size_t pos) {
          if (a[0] <= 0) {
            domain_error("log of a non-positive number", pos);
          }
          return std::log10(a[0]);
        }}},
      {"pow", {2, 2, [](const Args& a, std::size_t) { return std::pow(a[0], a[1]); }}},
      {"min",
       {1, kVariadic,
        [](const Args& a, std::size_t) { return *std::min_element(a.begin(), a.end()); }}},
      {"max",
       {1, kVariadic,
        [](const Args& a, std::size_t) { return *std::max_element(a.begin(), a.end()); }}},
  };
  return table;
}

std::optional<double> constant(const std::string& name) {
  if (name == "pi") {
    return kPi;
  }
  if (name == "e") {
    return kE;
  }
  return std::nullopt;
}

std::string arity_text(const Builtin& b) {
  if (b.max_args == kVariadic) {
    return "at least " + std::to_string(b.min_args);
  }
  if (b.min_args == b.max_args) {
    return std::to_string(b.min_args);
  }
  return std::to_string(b.min_args) + " to " + std::to_string(b.max_args);
}

double check_finite(double value, std::size_t position) {
  if (!std::isfinite(value)) {
    domain_error("result is not a finite number", position);
  }
  return value;
}

double eval_binary(char op, double lhs, double rhs, std::size_t position) {
  switch (op) {
    case '+':
      return lhs + rhs;
    case '-':
      return lhs - rhs;
    case '*':
      return lhs * rhs;
    case '/':
      if (rhs == 0.0) {
        throw CalcError(CalcError::Kind::DivisionByZero, "cannot divide by zero", position);
      }
      return lhs / rhs;
    case '%':
      if (rhs == 0.0) {
        throw CalcError(CalcError::Kind::DivisionByZero, "cannot take remainder by zero", position);
      }
      return std::fmod(lhs, rhs);
    case '^':
      return std::pow(lhs, rhs);
    default:
      throw CalcError(CalcError::Kind::Syntax, std::string("unknown operator '") + op + "'",
                      position);
  }
}

}  // namespace

std::optional<double> Context::get(const std::string& name) const {
  const auto it = variables_.find(name);
  if (it == variables_.end()) {
    return std::nullopt;
  }
  return it->second;
}

bool is_reserved_name(std::string_view name) {
  const std::string key(name);
  return constant(key).has_value() || builtins().count(key) > 0;
}

double evaluate(const Node& node, Context& context) {
  switch (node.kind) {
    case Node::Kind::NumberLiteral:
      return node.value;

    case Node::Kind::Variable: {
      if (auto value = constant(node.name)) {
        return *value;
      }
      if (auto value = context.get(node.name)) {
        return *value;
      }
      if (builtins().count(node.name) > 0) {
        throw CalcError(CalcError::Kind::Syntax,
                        "'" + node.name + "' is a function; call it like " + node.name + "(x)",
                        node.position);
      }
      throw CalcError(CalcError::Kind::UnknownIdentifier, "'" + node.name + "' is not defined",
                      node.position);
    }

    case Node::Kind::UnaryOp: {
      const double operand = evaluate(*node.children[0], context);
      return node.op == '-' ? -operand : operand;
    }

    case Node::Kind::BinaryOp: {
      const double lhs = evaluate(*node.children[0], context);
      const double rhs = evaluate(*node.children[1], context);
      return check_finite(eval_binary(node.op, lhs, rhs, node.position), node.position);
    }

    case Node::Kind::FunctionCall: {
      const auto it = builtins().find(node.name);
      if (it == builtins().end()) {
        throw CalcError(CalcError::Kind::UnknownIdentifier,
                        "'" + node.name + "' is not a known function", node.position);
      }
      const Builtin& builtin = it->second;
      const std::size_t count = node.children.size();
      if (count < builtin.min_args || count > builtin.max_args) {
        throw CalcError(CalcError::Kind::Arity,
                        node.name + " takes " + arity_text(builtin) + " argument(s), got " +
                            std::to_string(count),
                        node.position);
      }
      Args args;
      args.reserve(count);
      for (const auto& child : node.children) {
        args.push_back(evaluate(*child, context));
      }
      return check_finite(builtin.fn(args, node.position), node.position);
    }

    case Node::Kind::Assignment: {
      if (is_reserved_name(node.name)) {
        throw CalcError(CalcError::Kind::Syntax, "cannot assign to built-in '" + node.name + "'",
                        node.position);
      }
      const double value = evaluate(*node.children[0], context);
      context.set(node.name, value);
      return value;
    }
  }
  throw CalcError(CalcError::Kind::Syntax, "unknown expression", node.position);
}

double evaluate(std::string_view input, Context& context) {
  const auto tree = parse(tokenize(input));
  const double result = evaluate(*tree, context);
  context.set("ans", result);
  return result;
}

}  // namespace calc
