#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace calc {

// One node of the expression tree. A single tagged struct keeps the tree easy
// to build, walk and print; which fields are used depends on `kind`:
//
//   NumberLiteral  value
//   Variable       name
//   UnaryOp        op ('+' or '-'), children[0]
//   BinaryOp       op ('+', '-', '*', '/', '%', '^'), children[0..1]
//   FunctionCall   name, children = arguments
//   Assignment     name, children[0]
struct Node {
  enum class Kind {
    NumberLiteral,
    Variable,
    UnaryOp,
    BinaryOp,
    FunctionCall,
    Assignment,
  };

  Kind kind;
  std::size_t position = 0;  // column of the token that introduced the node
  double value = 0.0;
  char op = '\0';
  std::string name;
  std::vector<std::unique_ptr<Node>> children;

  static std::unique_ptr<Node> number(double value, std::size_t position);
  static std::unique_ptr<Node> variable(std::string name, std::size_t position);
  static std::unique_ptr<Node> unary(char op, std::unique_ptr<Node> operand, std::size_t position);
  static std::unique_ptr<Node> binary(char op, std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs,
                                      std::size_t position);
  static std::unique_ptr<Node> call(std::string name, std::vector<std::unique_ptr<Node>> args,
                                    std::size_t position);
  static std::unique_ptr<Node> assignment(std::string name, std::unique_ptr<Node> value,
                                          std::size_t position);
};

// Fully parenthesized rendering for debugging and tests:
// "2 + 3 * 4" -> "(2 + (3 * 4))", "-x" -> "(-x)", "max(1, 2)" -> "max(1, 2)".
std::string to_string(const Node& node);

}  // namespace calc
