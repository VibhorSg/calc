#include "calc/tokenizer.h"

#include <cctype>
#include <cstdlib>
#include <string>

#include "calc/error.h"

namespace calc {

namespace {

bool is_digit(char c) {
  return std::isdigit(static_cast<unsigned char>(c)) != 0;
}
bool is_ident_start(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_';
}
bool is_ident_char(char c) {
  return is_ident_start(c) || is_digit(c);
}
bool is_space(char c) {
  return std::isspace(static_cast<unsigned char>(c)) != 0;
}

// Lexes a number starting at `start`: digits, an optional fraction and an
// optional exponent. Returns the index one past the number.
std::size_t lex_number(std::string_view input, std::size_t start) {
  std::size_t i = start;
  bool has_digits = false;
  while (i < input.size() && is_digit(input[i])) {
    ++i;
    has_digits = true;
  }
  if (i < input.size() && input[i] == '.') {
    ++i;
    while (i < input.size() && is_digit(input[i])) {
      ++i;
      has_digits = true;
    }
  }
  if (!has_digits) {
    throw CalcError(CalcError::Kind::Syntax, "expected a digit", start);
  }
  if (i < input.size() && (input[i] == 'e' || input[i] == 'E')) {
    const std::size_t exponent_start = i;
    ++i;
    if (i < input.size() && (input[i] == '+' || input[i] == '-')) {
      ++i;
    }
    if (i >= input.size() || !is_digit(input[i])) {
      throw CalcError(CalcError::Kind::Syntax, "malformed exponent in number", exponent_start);
    }
    while (i < input.size() && is_digit(input[i])) {
      ++i;
    }
  }
  // A second '.' or a letter glued to the number (e.g. "1.2.3", "2x") is a
  // malformed number rather than two tokens.
  if (i < input.size() && (input[i] == '.' || is_ident_start(input[i]))) {
    throw CalcError(CalcError::Kind::Syntax, "malformed number", start);
  }
  return i;
}

}  // namespace

std::string_view to_string(TokenKind kind) {
  switch (kind) {
    case TokenKind::Number:
      return "number";
    case TokenKind::Identifier:
      return "identifier";
    case TokenKind::Plus:
      return "'+'";
    case TokenKind::Minus:
      return "'-'";
    case TokenKind::Star:
      return "'*'";
    case TokenKind::Slash:
      return "'/'";
    case TokenKind::Percent:
      return "'%'";
    case TokenKind::Caret:
      return "'^'";
    case TokenKind::LParen:
      return "'('";
    case TokenKind::RParen:
      return "')'";
    case TokenKind::Comma:
      return "','";
    case TokenKind::Assign:
      return "'='";
    case TokenKind::End:
      return "end of input";
  }
  return "token";
}

std::vector<Token> tokenize(std::string_view input) {
  std::vector<Token> tokens;
  std::size_t i = 0;
  while (i < input.size()) {
    const char c = input[i];
    if (is_space(c)) {
      ++i;
      continue;
    }
    if (is_digit(c) || c == '.') {
      const std::size_t end = lex_number(input, i);
      std::string text(input.substr(i, end - i));
      const double value = std::strtod(text.c_str(), nullptr);
      tokens.push_back({TokenKind::Number, std::move(text), value, i});
      i = end;
      continue;
    }
    if (is_ident_start(c)) {
      std::size_t end = i + 1;
      while (end < input.size() && is_ident_char(input[end])) {
        ++end;
      }
      tokens.push_back({TokenKind::Identifier, std::string(input.substr(i, end - i)), 0.0, i});
      i = end;
      continue;
    }
    TokenKind kind{};
    switch (c) {
      case '+':
        kind = TokenKind::Plus;
        break;
      case '-':
        kind = TokenKind::Minus;
        break;
      case '*':
        kind = TokenKind::Star;
        break;
      case '/':
        kind = TokenKind::Slash;
        break;
      case '%':
        kind = TokenKind::Percent;
        break;
      case '^':
        kind = TokenKind::Caret;
        break;
      case '(':
        kind = TokenKind::LParen;
        break;
      case ')':
        kind = TokenKind::RParen;
        break;
      case ',':
        kind = TokenKind::Comma;
        break;
      case '=':
        kind = TokenKind::Assign;
        break;
      default:
        throw CalcError(CalcError::Kind::Syntax, std::string("unexpected character '") + c + "'",
                        i);
    }
    tokens.push_back({kind, std::string(1, c), 0.0, i});
    ++i;
  }
  tokens.push_back({TokenKind::End, "", 0.0, input.size()});
  return tokens;
}

}  // namespace calc
