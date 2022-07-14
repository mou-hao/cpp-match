cmake_minimum_required(VERSION 3.14)
project(cpp_match)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

macro(add_test_target target_name)
  add_executable(
    ${target_name}
    test/${target_name}.cpp
  )
  target_link_libraries(
    ${target_name}
    GTest::gtest_main
  )
  gtest_discover_tests(${target_name})
endmacro()

add_test_target(integral)
