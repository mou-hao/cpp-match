#include "match.hpp"
#include <gtest/gtest.h>

using namespace mat;

TEST(IntegralTest, ExprMatch) {
  EXPECT_THROW({
    match(1) (
      pattern(2) = []() {},
      pattern(1) = []() { throw 1; });
  }, int);
}

TEST(IntegralTest, WildcardMatch) {
  EXPECT_THROW({
    match(1) (
      pattern(_) = []() { throw 1; },
      pattern(1) = []() {});
  }, int);
}

TEST(IntegralTest, CaptureMatch) {
  EXPECT_THROW({
    match(1) (
      pattern(cpt) = [](auto x) {
        EXPECT_EQ(x, 1);
        throw 1;
      },
      pattern(1) = []() {});
  }, int);
}

TEST(IntegralTest, NonVoidMatch) {
  int x = match(1) (
    pattern(1) = []() { return 100; },
    pattern(2) = []() { return 200; });
  EXPECT_EQ(x, 100);
}

TEST(IntegralTest, VoidNoMatch) {
  EXPECT_NO_THROW({
    match(1) (
      pattern(2) = []() {},
      pattern(3) = []() {});
  });
}

TEST(IntegralTest, NonVoidNoMatch) {
  EXPECT_THROW({
    match(1) (
      pattern(2) = []() { return 2; },
      pattern(3) = []() { return 2; });
  }, exceptions::NonVoidNoMatch);
}

TEST(IntegralTest, AltPattern) {
  EXPECT_THROW({
    match(1) (
      pattern(0) | pattern(1) = []() { throw 1; },
      pattern(2) = []() {});
  }, int);
}

TEST(IntegralTest, CharType) {
  EXPECT_THROW({
    match('a') (
      pattern(0) = []() {},
      pattern('b'-1) = []() { throw 1; });
  }, int);
}
