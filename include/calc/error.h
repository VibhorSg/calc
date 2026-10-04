#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>

namespace calc {

// Every failure the library reports is a CalcError. The position is the
// 0-based column in the input string that the error points at.
class CalcError : public std::runtime_error {
 public:
  enum class Kind {
    Syntax,
    UnknownIdentifier,
    DivisionByZero,
    Domain,
    Arity,
  };

  CalcError(Kind kind, const std::string& message, std::size_t position);

  Kind kind() const noexcept { return kind_; }
  std::size_t position() const noexcept { return position_; }

 private:
  Kind kind_;
  std::size_t position_;
};

// Human-readable name of an error kind, e.g. "syntax error".
std::string_view to_string(CalcError::Kind kind);

// Formats an error for display:
//
//   syntax error: unexpected ')'
//     2 + )
//         ^
std::string format_error(std::string_view input, const CalcError& error);

}  // namespace calc
