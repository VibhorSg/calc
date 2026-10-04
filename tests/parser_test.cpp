#include "calc/parser.h"

#include <gtest/gtest.h>

#include <string>

#include "calc/error.h"
#include "calc/tokenizer.h"

namespace calc {
namespace {

std::string parsed(const char* input) {
  return to_string(*parse(tokenize(input)));
}

CalcError parse_error(const char* input) {
  try {
    parse(tokenize(input));
  } catch (const CalcError& e) {
    return e;
  }
  ADD_FAILURE() << "expected CalcError for: " << input;
  return CalcError(CalcError::Kind::Syntax, "", 0);
}

TEST(ParserTest, PrecedenceAndAssociativity) {
  const struct {
    const char* input;
    const char* expected;
  } cases[] = {
      {"1 + 2 * 3", "(1 + (2 * 3))"},
      {"1 - 2 - 3", "((1 - 2) - 3)"},
      {"8 / 4 / 2", "((8 / 4) / 2)"},
      {"7 % 3 * 2", "((7 % 3) * 2)"},
      {"2 * (3 + 4)", "(2 * (3 + 4))"},
      {"2 ^ 3 ^ 2", "(2 ^ (3 ^ 2))"},
      {"-2 ^ 2", "(-(2 ^ 2))"},
      {"2 ^ -1", "(2 ^ (-1))"},
      {"--3", "(-(-3))"},
      {"+4", "(+4)"},
      {"-(1 + 2) * 3", "((-(1 + 2)) * 3)"},
      {"2 * -3", "(2 * (-3))"},
      {"((5))", "5"},
  };
  for (const auto& c : cases) {
    EXPECT_EQ(parsed(c.input), c.expected) << c.input;
  }
}

TEST(ParserTest, VariablesAndAssignment) {
  EXPECT_EQ(parsed("x"), "x");
  EXPECT_EQ(parsed("x = 3"), "(x = 3)");
  EXPECT_EQ(parsed("y = x * 2 + 1"), "(y = ((x * 2) + 1))");
}

TEST(ParserTest, FunctionCalls) {
  EXPECT_EQ(parsed("sin(0)"), "sin(0)");
  EXPECT_EQ(parsed("max(1, 2, 3)"), "max(1, 2, 3)");
  EXPECT_EQ(parsed("f()"), "f()");
  EXPECT_EQ(parsed("sqrt(16) + pi"), "(sqrt(16) + pi)");
  EXPECT_EQ(parsed("pow(2, 1 + 1) ^ 2"), "(pow(2, (1 + 1)) ^ 2)");
}

TEST(ParserTest, NodePositions) {
  const auto tree = parse(tokenize("12 + foo(3)"));
  EXPECT_EQ(tree->position, 3U);
  EXPECT_EQ(tree->children[0]->position, 0U);
  EXPECT_EQ(tree->children[1]->position, 5U);
}

TEST(ParserTest, EmptyInput) {
  const auto e = parse_error("   ");
  EXPECT_EQ(e.kind(), CalcError::Kind::Syntax);
  EXPECT_STREQ(e.what(), "empty expression");
}

TEST(ParserTest, EmptyTokenListIsAnError) {
  EXPECT_THROW(parse({}), CalcError);
}

TEST(ParserTest, UnexpectedToken) {
  const auto e = parse_error("2 + * 3");
  EXPECT_EQ(e.position(), 4U);
  EXPECT_STREQ(e.what(), "unexpected '*'");
}

TEST(ParserTest, DanglingOperator) {
  const auto e = parse_error("2 +");
  EXPECT_EQ(e.position(), 3U);
  EXPECT_STREQ(e.what(), "unexpected end of input");
}

TEST(ParserTest, MissingClosingParen) {
  const auto e = parse_error("(1 + 2");
  EXPECT_EQ(e.position(), 6U);
  EXPECT_STREQ(e.what(), "missing ')' to close '(' at column 1");
}

TEST(ParserTest, StrayClosingParen) {
  EXPECT_EQ(parse_error(")").position(), 0U);
  EXPECT_EQ(parse_error("1 + 2)").position(), 5U);
}

TEST(ParserTest, TrailingInput) {
  const auto e = parse_error("1 2");
  EXPECT_EQ(e.position(), 2U);
  EXPECT_STREQ(e.what(), "unexpected number");
}

TEST(ParserTest, BadFunctionCalls) {
  EXPECT_EQ(parse_error("max(1,)").position(), 6U);
  EXPECT_EQ(parse_error("sin(").position(), 4U);
  EXPECT_EQ(parse_error("max(1 2)").position(), 6U);
}

TEST(ParserTest, AssignmentNeedsAValue) {
  EXPECT_EQ(parse_error("x =").position(), 3U);
  EXPECT_EQ(parse_error("1 = 2").position(), 2U);
}

}  // namespace
}  // namespace calc
