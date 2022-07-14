#include "match.hpp"
#include <gtest/gtest.h>
#include <variant>

using namespace mat;

TEST(VarTest, VarMatch) {
  std::variant<int, bool> x{1};
  auto y = match(x) (
    pattern(var<bool>) = [](auto) { return 0; },
    pattern(var<int>)  = [](auto n) { return n; });
  EXPECT_EQ(y, 1);
}

TEST(VarTest, VarMixAndMatch) {
  std::variant<int, bool> x{1};
  auto y = match(x) (
    pattern(var<bool>) = [](auto) { return 0; },
    pattern(_)         = []() { return 1; });
  EXPECT_EQ(y, 1);
}
