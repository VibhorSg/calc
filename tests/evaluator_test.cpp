#include "calc/evaluator.h"

#include <gtest/gtest.h>

#include <cmath>
#include <string>

#include "calc/error.h"

namespace calc {
namespace {

constexpr double kTolerance = 1e-9;
constexpr double kPi = 3.14159265358979323846;

double eval(const std::string& input) {
  Context context;
  return evaluate(input, context);
}

CalcError eval_error(const std::string& input, Context& context) {
  try {
    evaluate(input, context);
  } catch (const CalcError& e) {
    return e;
  }
  ADD_FAILURE() << "expected CalcError for: " << input;
  return CalcError(CalcError::Kind::Syntax, "", 0);
}

CalcError eval_error(const std::string& input) {
  Context context;
  return eval_error(input, context);
}

// The acceptance criteria from the Core Engine epic.
TEST(EvaluatorTest, EpicAcceptanceCriteria) {
  EXPECT_DOUBLE_EQ(eval("2 + 3 * 4"), 14.0);
  EXPECT_DOUBLE_EQ(eval("2 ^ 3 ^ 2"), 512.0);
  EXPECT_DOUBLE_EQ(eval("-(1+2)*3"), -9.0);
  EXPECT_NEAR(eval("sqrt(16) + pi"), 4.0 + kPi, kTolerance);
  EXPECT_EQ(eval_error("1/0").kind(), CalcError::Kind::DivisionByZero);
}

TEST(EvaluatorTest, Arithmetic) {
  EXPECT_DOUBLE_EQ(eval("1 + 2"), 3.0);
  EXPECT_DOUBLE_EQ(eval("10 - 4 - 3"), 3.0);
  EXPECT_DOUBLE_EQ(eval("6 * 7"), 42.0);
  EXPECT_DOUBLE_EQ(eval("7 / 2"), 3.5);
  EXPECT_DOUBLE_EQ(eval("7 % 3"), 1.0);
  EXPECT_DOUBLE_EQ(eval("-7 % 3"), -1.0);
  EXPECT_DOUBLE_EQ(eval("5.5 % 2"), 1.5);
  EXPECT_DOUBLE_EQ(eval("2 ^ 10"), 1024.0);
  EXPECT_DOUBLE_EQ(eval("2 ^ -1"), 0.5);
  EXPECT_DOUBLE_EQ(eval("-2 ^ 2"), -4.0);
  EXPECT_DOUBLE_EQ(eval("(-2) ^ 2"), 4.0);
  EXPECT_DOUBLE_EQ(eval("+5"), 5.0);
  EXPECT_DOUBLE_EQ(eval("--5"), 5.0);
  EXPECT_DOUBLE_EQ(eval("1.5e3 + .5"), 1500.5);
}

TEST(EvaluatorTest, Constants) {
  EXPECT_NEAR(eval("pi"), kPi, kTolerance);
  EXPECT_NEAR(eval("e"), std::exp(1.0), kTolerance);
}

TEST(EvaluatorTest, BuiltinsMatchCmath) {
  EXPECT_NEAR(eval("sin(1)"), std::sin(1.0), kTolerance);
  EXPECT_NEAR(eval("cos(1)"), std::cos(1.0), kTolerance);
  EXPECT_NEAR(eval("tan(1)"), std::tan(1.0), kTolerance);
  EXPECT_NEAR(eval("sqrt(2)"), std::sqrt(2.0), kTolerance);
  EXPECT_NEAR(eval("log(1000)"), 3.0, kTolerance);
  EXPECT_NEAR(eval("ln(e)"), 1.0, kTolerance);
  EXPECT_NEAR(eval("abs(-3.5)"), 3.5, kTolerance);
  EXPECT_NEAR(eval("pow(2, 0.5)"), std::sqrt(2.0), kTolerance);
  EXPECT_DOUBLE_EQ(eval("min(3, -1, 2)"), -1.0);
  EXPECT_DOUBLE_EQ(eval("max(3, -1, 2)"), 3.0);
  EXPECT_DOUBLE_EQ(eval("max(7)"), 7.0);
  EXPECT_NEAR(eval("sin(pi / 2)"), 1.0, kTolerance);
}

TEST(EvaluatorTest, VariablesAndAssignment) {
  Context context;
  EXPECT_DOUBLE_EQ(evaluate("x = 3", context), 3.0);
  EXPECT_DOUBLE_EQ(evaluate("x * 2", context), 6.0);
  EXPECT_DOUBLE_EQ(evaluate("x = x + 1", context), 4.0);
  EXPECT_DOUBLE_EQ(context.get("x").value_or(-1.0), 4.0);
  EXPECT_DOUBLE_EQ(evaluate("y = 2 * x", context), 8.0);
  EXPECT_EQ(context.variables().count("y"), 1U);
}

TEST(EvaluatorTest, AnsHoldsLastResult) {
  Context context;
  evaluate("2 + 3", context);
  EXPECT_DOUBLE_EQ(context.get("ans").value_or(-1.0), 5.0);
  EXPECT_DOUBLE_EQ(evaluate("ans * 2", context), 10.0);
  EXPECT_DOUBLE_EQ(evaluate("ans + 1", context), 11.0);
}

TEST(EvaluatorTest, FailedEvaluationLeavesAnsUnchanged) {
  Context context;
  evaluate("7", context);
  eval_error("1 / 0", context);
  EXPECT_DOUBLE_EQ(context.get("ans").value_or(-1.0), 7.0);
}

TEST(EvaluatorTest, AnsIsUndefinedBeforeFirstResult) {
  EXPECT_EQ(eval_error("ans").kind(), CalcError::Kind::UnknownIdentifier);
}

TEST(EvaluatorTest, EvaluatesATreeDirectly) {
  Context context;
  const auto tree = Node::binary('+', Node::number(1, 0), Node::number(2, 2), 1);
  EXPECT_DOUBLE_EQ(evaluate(*tree, context), 3.0);
  EXPECT_FALSE(context.get("ans").has_value());
}

TEST(EvaluatorTest, DivisionByZero) {
  const auto e = eval_error("4 / (2 - 2)");
  EXPECT_EQ(e.kind(), CalcError::Kind::DivisionByZero);
  EXPECT_EQ(e.position(), 2U);
  EXPECT_EQ(eval_error("5 % 0").kind(), CalcError::Kind::DivisionByZero);
}

TEST(EvaluatorTest, DomainErrors) {
  const auto e = eval_error("1 + sqrt(-1)");
  EXPECT_EQ(e.kind(), CalcError::Kind::Domain);
  EXPECT_EQ(e.position(), 4U);
  EXPECT_EQ(eval_error("ln(0)").kind(), CalcError::Kind::Domain);
  EXPECT_EQ(eval_error("log(-5)").kind(), CalcError::Kind::Domain);
  EXPECT_EQ(eval_error("(-8) ^ 0.5").kind(), CalcError::Kind::Domain);
  EXPECT_EQ(eval_error("10 ^ 400").kind(), CalcError::Kind::Domain);
  EXPECT_EQ(eval_error("1e308 * 10").kind(), CalcError::Kind::Domain);
}

TEST(EvaluatorTest, UnknownIdentifiers) {
  const auto e = eval_error("2 * foo");
  EXPECT_EQ(e.kind(), CalcError::Kind::UnknownIdentifier);
  EXPECT_EQ(e.position(), 4U);
  EXPECT_EQ(eval_error("foo(1)").kind(), CalcError::Kind::UnknownIdentifier);
}

TEST(EvaluatorTest, FunctionUsedWithoutParentheses) {
  const auto e = eval_error("sqrt + 1");
  EXPECT_EQ(e.kind(), CalcError::Kind::Syntax);
  EXPECT_EQ(e.position(), 0U);
}

TEST(EvaluatorTest, WrongArity) {
  const auto e = eval_error("sin(1, 2)");
  EXPECT_EQ(e.kind(), CalcError::Kind::Arity);
  EXPECT_STREQ(e.what(), "sin takes 1 argument(s), got 2");
  EXPECT_EQ(eval_error("pow(2)").kind(), CalcError::Kind::Arity);
  EXPECT_STREQ(eval_error("max()").what(), "max takes at least 1 argument(s), got 0");
}

TEST(EvaluatorTest, CannotAssignToBuiltins) {
  EXPECT_EQ(eval_error("pi = 3").kind(), CalcError::Kind::Syntax);
  EXPECT_EQ(eval_error("e = 1").kind(), CalcError::Kind::Syntax);
  EXPECT_EQ(eval_error("sin = 1").kind(), CalcError::Kind::Syntax);
}

TEST(EvaluatorTest, ReservedNames) {
  EXPECT_TRUE(is_reserved_name("pi"));
  EXPECT_TRUE(is_reserved_name("sqrt"));
  EXPECT_FALSE(is_reserved_name("ans"));
  EXPECT_FALSE(is_reserved_name("x"));
}

TEST(EvaluatorTest, SyntaxErrorsPropagate) {
  EXPECT_EQ(eval_error("2 +").kind(), CalcError::Kind::Syntax);
  EXPECT_EQ(eval_error("2 # 3").kind(), CalcError::Kind::Syntax);
}

}  // namespace
}  // namespace calc
