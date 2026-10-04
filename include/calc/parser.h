#pragma once

#include <memory>
#include <vector>

#include "calc/ast.h"
#include "calc/tokenizer.h"

namespace calc {

// Builds an expression tree from tokens produced by tokenize(). Grammar,
// lowest to highest precedence:
//
//   statement  := IDENT '=' expression | expression
//   expression := term (('+' | '-') term)*
//   term       := unary (('*' | '/' | '%') unary)*
//   unary      := ('+' | '-') unary | power
//   power      := primary ('^' unary)?          right-associative
//   primary    := NUMBER | IDENT | IDENT '(' args? ')' | '(' expression ')'
//
// So "-2 ^ 2" is "-(2 ^ 2)" and "2 ^ -1" is "2 ^ (-1)". Throws
// CalcError{Syntax} on empty input, unexpected tokens, a missing ')' or
// trailing input.
std::unique_ptr<Node> parse(const std::vector<Token>& tokens);

}  // namespace calc
