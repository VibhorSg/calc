#include "calc/format.h"

#include <gtest/gtest.h>

namespace calc {
namespace {

TEST(FormatNumberTest, WholeNumbersHaveNoDecimalPoint) {
  EXPECT_EQ(format_number(14.0), "14");
  EXPECT_EQ(format_number(-3.0), "-3");
  EXPECT_EQ(format_number(512.0), "512");
  EXPECT_EQ(format_number(123456789012345.0), "123456789012345");
}

TEST(FormatNumberTest, Zero) {
  EXPECT_EQ(format_number(0.0), "0");
  EXPECT_EQ(format_number(-0.0), "0");
}

TEST(FormatNumberTest, Fractions) {
  EXPECT_EQ(format_number(0.5), "0.5");
  EXPECT_EQ(format_number(0.1 + 0.2), "0.3");
  EXPECT_EQ(format_number(3.14159265358979323846), "3.14159265358979");
  EXPECT_EQ(format_number(-2.25), "-2.25");
}

TEST(FormatNumberTest, VeryLargeAndSmall) {
  EXPECT_EQ(format_number(1e20), "1e+20");
  EXPECT_EQ(format_number(1e-20), "1e-20");
}

}  // namespace
}  // namespace calc
