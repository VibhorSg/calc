#include <gtest/gtest.h>

#include <string>

#include "calc/version.h"

TEST(SmokeTest, VersionIsSet) { EXPECT_FALSE(std::string(calc::kVersion).empty()); }
