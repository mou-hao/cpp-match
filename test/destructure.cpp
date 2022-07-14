#include "match.hpp"
#include <gtest/gtest.h>
#include <tuple>
#include <utility>

using namespace mat;

TEST(DsTest, TupleExprMatch) {
  EXPECT_THROW({
    match(std::make_tuple(1, false, 2.0)) (
      pattern(ds(1, true, 2.0))  = []() {},
      pattern(ds(1, false, 2.0)) = []() { throw 1; });
  }, int);
}

TEST(DsTest, TupleWildcardMatch) {
  int x = match(std::make_tuple(1, false, 2.0)) (
    pattern(ds(1, true, 2.0))  = []() { return 0; },
    pattern(ds(1, false, _))   = []() { return 1; },
    pattern(ds(1, false, 2.0)) = []() { return 2; });
  EXPECT_EQ(x, 1);
}

TEST(DsTest, TupleCaptureMatch) {
  int x = match(std::make_tuple(1, false, 2.0)) (
    pattern(ds(1, true, cpt))  = [](auto) { return 0; },
    pattern(ds(_, _, 1.0))     = []() { return 1; },
    pattern(ds(cpt, false, _)) = [](auto n) { return n+1; });
  EXPECT_EQ(x, 2);
}

TEST(DsTest, MixAndMatch) {
  int x = match(std::make_tuple(1, false)) (
    pattern(ds(_, true)) = []() { return 0; },
    pattern(ds(2, cpt))  = [](auto b) { return b ? 1 : 2; },
    pattern(_)           = []() { return 3; });
  EXPECT_EQ(x, 3);
}

TEST(DsTest, PairIsTuple) {
  int x = match(std::make_pair(true, 2)) (
    pattern(ds(false, _)) = []() { return 0; },
    pattern(ds(_, cpt))   = [](auto n) { return n; });
  EXPECT_EQ(x, 2);
}
