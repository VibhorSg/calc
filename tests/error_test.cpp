#include "calc/error.h"

#include <gtest/gtest.h>

namespace calc {
namespace {

TEST(CalcErrorTest, StoresKindMessageAndPosition) {
  const CalcError e(CalcError::Kind::Domain, "sqrt of a negative number", 4);
  EXPECT_EQ(e.kind(), CalcError::Kind::Domain);
  EXPECT_STREQ(e.what(), "sqrt of a negative number");
  EXPECT_EQ(e.position(), 4U);
}

TEST(CalcErrorTest, IsARuntimeError) {
  try {
    throw CalcError(CalcError::Kind::Syntax, "boom", 0);
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "boom");
    return;
  }
  FAIL() << "CalcError was not caught as std::runtime_error";
}

TEST(CalcErrorTest, KindNames) {
  EXPECT_EQ(to_string(CalcError::Kind::Syntax), "syntax error");
  EXPECT_EQ(to_string(CalcError::Kind::UnknownIdentifier), "unknown identifier");
  EXPECT_EQ(to_string(CalcError::Kind::DivisionByZero), "division by zero");
  EXPECT_EQ(to_string(CalcError::Kind::Domain), "domain error");
  EXPECT_EQ(to_string(CalcError::Kind::Arity), "wrong number of arguments");
}

TEST(FormatErrorTest, CaretAtFirstColumn) {
  const CalcError e(CalcError::Kind::Syntax, "unexpected ')'", 0);
  EXPECT_EQ(format_error(")", e), "syntax error: unexpected ')'\n  )\n  ^");
}

TEST(FormatErrorTest, CaretInMiddle) {
  const CalcError e(CalcError::Kind::DivisionByZero, "cannot divide by zero", 2);
  EXPECT_EQ(format_error("1 / 0", e), "division by zero: cannot divide by zero\n  1 / 0\n    ^");
}

TEST(FormatErrorTest, CaretJustPastEndOfInput) {
  const CalcError e(CalcError::Kind::Syntax, "unexpected end of input", 3);
  EXPECT_EQ(format_error("2 +", e), "syntax error: unexpected end of input\n  2 +\n     ^");
}

TEST(FormatErrorTest, PositionBeyondInputIsClamped) {
  const CalcError e(CalcError::Kind::Syntax, "x", 99);
  EXPECT_EQ(format_error("ab", e), "syntax error: x\n  ab\n    ^");
}

}  // namespace
}  // namespace calc
