#include "calc/parser.h"

#include <string>
#include <utility>

#include "calc/error.h"

namespace calc {

namespace {

class Parser {
 public:
  explicit Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

  std::unique_ptr<Node> parse_all() {
    if (tokens_.empty() || peek().kind == TokenKind::End) {
      throw CalcError(CalcError::Kind::Syntax, "empty expression",
                      tokens_.empty() ? 0 : peek().position);
    }
    auto node = statement();
    if (peek().kind != TokenKind::End) {
      throw unexpected(peek());
    }
    return node;
  }

 private:
  const Token& peek(std::size_t ahead = 0) const {
    const std::size_t index = pos_ + ahead;
    return index < tokens_.size() ? tokens_[index] : tokens_.back();
  }

  const Token& advance() {
    const Token& token = peek();
    if (pos_ < tokens_.size() - 1) {
      ++pos_;
    }
    return token;
  }

  bool match(TokenKind kind) {
    if (peek().kind == kind) {
      advance();
      return true;
    }
    return false;
  }

  static CalcError unexpected(const Token& token) {
    if (token.kind == TokenKind::End) {
      return CalcError(CalcError::Kind::Syntax, "unexpected end of input", token.position);
    }
    return CalcError(CalcError::Kind::Syntax, "unexpected " + std::string(to_string(token.kind)),
                     token.position);
  }

  std::unique_ptr<Node> statement() {
    if (peek().kind == TokenKind::Identifier && peek(1).kind == TokenKind::Assign) {
      const Token& name = advance();
      advance();  // '='
      auto value = expression();
      return Node::assignment(name.text, std::move(value), name.position);
    }
    return expression();
  }

  std::unique_ptr<Node> expression() {
    auto lhs = term();
    while (peek().kind == TokenKind::Plus || peek().kind == TokenKind::Minus) {
      const Token& op = advance();
      auto rhs = term();
      lhs = Node::binary(op.text[0], std::move(lhs), std::move(rhs), op.position);
    }
    return lhs;
  }

  std::unique_ptr<Node> term() {
    auto lhs = unary();
    while (peek().kind == TokenKind::Star || peek().kind == TokenKind::Slash ||
           peek().kind == TokenKind::Percent) {
      const Token& op = advance();
      auto rhs = unary();
      lhs = Node::binary(op.text[0], std::move(lhs), std::move(rhs), op.position);
    }
    return lhs;
  }

  std::unique_ptr<Node> unary() {
    if (peek().kind == TokenKind::Plus || peek().kind == TokenKind::Minus) {
      const Token& op = advance();
      auto operand = unary();
      return Node::unary(op.text[0], std::move(operand), op.position);
    }
    return power();
  }

  std::unique_ptr<Node> power() {
    auto base = primary();
    if (peek().kind == TokenKind::Caret) {
      const Token& op = advance();
      auto exponent = unary();  // recursion through unary makes '^' right-associative
      return Node::binary('^', std::move(base), std::move(exponent), op.position);
    }
    return base;
  }

  std::unique_ptr<Node> primary() {
    const Token& token = peek();
    switch (token.kind) {
      case TokenKind::Number:
        advance();
        return Node::number(token.value, token.position);
      case TokenKind::Identifier:
        advance();
        if (match(TokenKind::LParen)) {
          return call(token);
        }
        return Node::variable(token.text, token.position);
      case TokenKind::LParen: {
        advance();
        auto inner = expression();
        expect_closing_paren(token);
        return inner;
      }
      default:
        throw unexpected(token);
    }
  }

  std::unique_ptr<Node> call(const Token& name) {
    std::vector<std::unique_ptr<Node>> args;
    if (!match(TokenKind::RParen)) {
      do {
        args.push_back(expression());
      } while (match(TokenKind::Comma));
      expect_closing_paren(name);
    }
    return Node::call(name.text, std::move(args), name.position);
  }

  void expect_closing_paren(const Token& opener) {
    if (match(TokenKind::RParen)) {
      return;
    }
    if (peek().kind == TokenKind::End) {
      throw CalcError(CalcError::Kind::Syntax,
                      "missing ')' to close '(' at column " + std::to_string(opener.position + 1),
                      peek().position);
    }
    throw unexpected(peek());
  }

  const std::vector<Token>& tokens_;
  std::size_t pos_ = 0;
};

}  // namespace

std::unique_ptr<Node> parse(const std::vector<Token>& tokens) {
  return Parser(tokens).parse_all();
}

}  // namespace calc
