#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace calc {

enum class TokenKind {
  Number,
  Identifier,
  Plus,
  Minus,
  Star,
  Slash,
  Percent,
  Caret,
  LParen,
  RParen,
  Comma,
  Assign,
  End,
};

struct Token {
  TokenKind kind;
  std::string text;      // exact source text of the token ("" for End)
  double value = 0.0;    // parsed value, only meaningful for Number
  std::size_t position;  // 0-based column of the first character
};

std::string_view to_string(TokenKind kind);

// Splits an expression into tokens. The result always ends with a single End
// token positioned at input.size(). Throws CalcError{Syntax} on an invalid
// character or a malformed number.
std::vector<Token> tokenize(std::string_view input);

}  // namespace calc
