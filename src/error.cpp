#include "calc/error.h"

#include <algorithm>

namespace calc {

CalcError::CalcError(Kind kind, const std::string& message, std::size_t position)
    : std::runtime_error(message), kind_(kind), position_(position) {}

std::string_view to_string(CalcError::Kind kind) {
  switch (kind) {
    case CalcError::Kind::Syntax:
      return "syntax error";
    case CalcError::Kind::UnknownIdentifier:
      return "unknown identifier";
    case CalcError::Kind::DivisionByZero:
      return "division by zero";
    case CalcError::Kind::Domain:
      return "domain error";
    case CalcError::Kind::Arity:
      return "wrong number of arguments";
  }
  return "error";
}

std::string format_error(std::string_view input, const CalcError& error) {
  std::string out;
  out += to_string(error.kind());
  out += ": ";
  out += error.what();
  out += "\n  ";
  out += input;
  out += "\n  ";
  const std::size_t column = std::min(error.position(), input.size());
  out.append(column, ' ');
  out += '^';
  return out;
}

}  // namespace calc
