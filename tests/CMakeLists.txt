find_package(GTest REQUIRED)

add_executable(
  tests
  matcher-test.cpp
)
target_link_libraries(
  tests
  lib${PROJECT_NAME}-matcher
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)
