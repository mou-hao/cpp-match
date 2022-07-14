#include "match.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace mat;

TEST(StringTest, ExprMatch) {
  EXPECT_THROW({
    match("this") (
      pattern("that") = []() {},
      pattern("this") = []() { throw 1; });
  }, int);
}

TEST(StringTest, StringConstChar) {
  auto x = "this";
  EXPECT_THROW({
    match(x) (
      pattern("that") = [] () {},
      pattern("this") = []() { throw 1; });
  }, int);
}

TEST(StringTest, WildcardMatch) {
  EXPECT_THROW({
    match("this") (
      pattern(_) = []() { throw 1; },
      pattern("this") = []() {});
  }, int);
}

TEST(StringTest, CaptureMatch) {
  EXPECT_THROW({
    match("this") (
      pattern(cpt) = [](auto x) {
        EXPECT_EQ(x[2], 'i');
        throw 1;
      },
      pattern("this") = []() {});
  }, int);
}

TEST(StringTest, NonVoidMatch) {
  std::string x = match("this") (
    pattern("that") = []() { return "that"; },
    pattern(_) = []() { return "wildcard"; });
  EXPECT_EQ(x, "wildcard");
}

TEST(StringTest, VoidNoMatch) {
  EXPECT_NO_THROW({
    match("this") (
      pattern("thi") = []() {},
      pattern("that") = []() {});
  });
}

TEST(StringTest, NonVoidNoMatch) {
  EXPECT_THROW({
    match("this") (
      pattern("thi") = []() { return 2; },
      pattern("that") = []() { return 2; });
  }, exceptions::NonVoidNoMatch);
}

TEST(StringTest, AltPattern) {
  EXPECT_THROW({
    match("this") (
      pattern("thi") | pattern("this") | pattern("that") = []() { throw 1; },
      pattern(_) = []() {});
  }, int);
}

