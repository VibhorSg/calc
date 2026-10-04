#include "calc/ast.h"

#include <utility>

#include "calc/format.h"

namespace calc {

std::unique_ptr<Node> Node::number(double value, std::size_t position) {
  auto node = std::make_unique<Node>();
  node->kind = Kind::NumberLiteral;
  node->value = value;
  node->position = position;
  return node;
}

std::unique_ptr<Node> Node::variable(std::string name, std::size_t position) {
  auto node = std::make_unique<Node>();
  node->kind = Kind::Variable;
  node->name = std::move(name);
  node->position = position;
  return node;
}

std::unique_ptr<Node> Node::unary(char op, std::unique_ptr<Node> operand, std::size_t position) {
  auto node = std::make_unique<Node>();
  node->kind = Kind::UnaryOp;
  node->op = op;
  node->children.push_back(std::move(operand));
  node->position = position;
  return node;
}

std::unique_ptr<Node> Node::binary(char op, std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs,
                                   std::size_t position) {
  auto node = std::make_unique<Node>();
  node->kind = Kind::BinaryOp;
  node->op = op;
  node->children.push_back(std::move(lhs));
  node->children.push_back(std::move(rhs));
  node->position = position;
  return node;
}

std::unique_ptr<Node> Node::call(std::string name, std::vector<std::unique_ptr<Node>> args,
                                 std::size_t position) {
  auto node = std::make_unique<Node>();
  node->kind = Kind::FunctionCall;
  node->name = std::move(name);
  node->children = std::move(args);
  node->position = position;
  return node;
}

std::unique_ptr<Node> Node::assignment(std::string name, std::unique_ptr<Node> value,
                                       std::size_t position) {
  auto node = std::make_unique<Node>();
  node->kind = Kind::Assignment;
  node->name = std::move(name);
  node->children.push_back(std::move(value));
  node->position = position;
  return node;
}

std::string to_string(const Node& node) {
  switch (node.kind) {
    case Node::Kind::NumberLiteral:
      return format_number(node.value);
    case Node::Kind::Variable:
      return node.name;
    case Node::Kind::UnaryOp:
      return "(" + std::string(1, node.op) + to_string(*node.children[0]) + ")";
    case Node::Kind::BinaryOp:
      return "(" + to_string(*node.children[0]) + " " + std::string(1, node.op) + " " +
             to_string(*node.children[1]) + ")";
    case Node::Kind::FunctionCall: {
      std::string out = node.name + "(";
      for (std::size_t i = 0; i < node.children.size(); ++i) {
        if (i > 0) {
          out += ", ";
        }
        out += to_string(*node.children[i]);
      }
      return out + ")";
    }
    case Node::Kind::Assignment:
      return "(" + node.name + " = " + to_string(*node.children[0]) + ")";
  }
  return "?";
}

}  // namespace calc
