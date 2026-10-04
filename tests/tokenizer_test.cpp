#include "calc/tokenizer.h"

#include <gtest/gtest.h>

#include <vector>

#include "calc/error.h"

namespace calc {
namespace {

std::vector<TokenKind> kinds(const std::vector<Token>& tokens) {
  std::vector<TokenKind> out;
  out.reserve(tokens.size());
  for (const auto& t : tokens) {
    out.push_back(t.kind);
  }
  return out;
}

CalcError tokenize_error(const char* input) {
  try {
    tokenize(input);
  } catch (const CalcError& e) {
    return e;
  }
  ADD_FAILURE() << "expected CalcError for: " << input;
  return CalcError(CalcError::Kind::Syntax, "", 0);
}

TEST(TokenizerTest, EmptyInputYieldsOnlyEnd) {
  const auto tokens = tokenize("");
  ASSERT_EQ(tokens.size(), 1U);
  EXPECT_EQ(tokens[0].kind, TokenKind::End);
  EXPECT_EQ(tokens[0].position, 0U);
}

TEST(TokenizerTest, WhitespaceOnlyYieldsEndAtInputSize) {
  const auto tokens = tokenize(" \t ");
  ASSERT_EQ(tokens.size(), 1U);
  EXPECT_EQ(tokens[0].position, 3U);
}

TEST(TokenizerTest, AllOperatorsAndPunctuation) {
  EXPECT_EQ(kinds(tokenize("+-*/%^(),=")),
            (std::vector<TokenKind>{TokenKind::Plus, TokenKind::Minus, TokenKind::Star,
                                    TokenKind::Slash, TokenKind::Percent, TokenKind::Caret,
                                    TokenKind::LParen, TokenKind::RParen, TokenKind::Comma,
                                    TokenKind::Assign, TokenKind::End}));
}

TEST(TokenizerTest, NumberFormats) {
  const struct {
    const char* text;
    double value;
  } cases[] = {{"42", 42.0},   {"3.14", 3.14},    {".5", 0.5},    {"5.", 5.0},
               {"1e-3", 1e-3}, {"2.5E+4", 2.5e4}, {"7e2", 700.0}, {"0", 0.0}};
  for (const auto& c : cases) {
    const auto tokens = tokenize(c.text);
    ASSERT_EQ(tokens.size(), 2U) << c.text;
    EXPECT_EQ(tokens[0].kind, TokenKind::Number) << c.text;
    EXPECT_DOUBLE_EQ(tokens[0].value, c.value) << c.text;
    EXPECT_EQ(tokens[0].text, c.text);
  }
}

TEST(TokenizerTest, Identifiers) {
  const auto tokens = tokenize("x _tmp sqrt var2");
  ASSERT_EQ(tokens.size(), 5U);
  EXPECT_EQ(tokens[0].text, "x");
  EXPECT_EQ(tokens[1].text, "_tmp");
  EXPECT_EQ(tokens[2].text, "sqrt");
  EXPECT_EQ(tokens[3].text, "var2");
  for (std::size_t i = 0; i < 4; ++i) {
    EXPECT_EQ(tokens[i].kind, TokenKind::Identifier);
  }
}

TEST(TokenizerTest, RecordsPositions) {
  const auto tokens = tokenize(" 12 +  x");
  ASSERT_EQ(tokens.size(), 4U);
  EXPECT_EQ(tokens[0].position, 1U);
  EXPECT_EQ(tokens[1].position, 4U);
  EXPECT_EQ(tokens[2].position, 7U);
  EXPECT_EQ(tokens[3].position, 8U);
}

TEST(TokenizerTest, MixedExpression) {
  EXPECT_EQ(kinds(tokenize("max(1, x) ^ 2")),
            (std::vector<TokenKind>{TokenKind::Identifier, TokenKind::LParen, TokenKind::Number,
                                    TokenKind::Comma, TokenKind::Identifier, TokenKind::RParen,
                                    TokenKind::Caret, TokenKind::Number, TokenKind::End}));
}

TEST(TokenizerTest, InvalidCharacterReportsPosition) {
  const auto e = tokenize_error("1 + # 2");
  EXPECT_EQ(e.kind(), CalcError::Kind::Syntax);
  EXPECT_EQ(e.position(), 4U);
  EXPECT_STREQ(e.what(), "unexpected character '#'");
}

TEST(TokenizerTest, MalformedNumbers) {
  EXPECT_EQ(tokenize_error("1.2.3").position(), 0U);
  EXPECT_EQ(tokenize_error("2x").position(), 0U);
  EXPECT_EQ(tokenize_error("4 + 1e").position(), 5U);
  EXPECT_EQ(tokenize_error("1e+").position(), 1U);
  EXPECT_EQ(tokenize_error(".").position(), 0U);
}

TEST(TokenizerTest, TokenKindNames) {
  EXPECT_EQ(to_string(TokenKind::Number), "number");
  EXPECT_EQ(to_string(TokenKind::Identifier), "identifier");
  EXPECT_EQ(to_string(TokenKind::RParen), "')'");
  EXPECT_EQ(to_string(TokenKind::End), "end of input");
}

}  // namespace
}  // namespace calc
