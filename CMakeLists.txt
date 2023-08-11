cmake_minimum_required(VERSION 3.14)
project(my_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  tree_test
  test/tree_test.cpp
)
target_compile_options(
  tree_test PRIVATE -Wall -Wextra -Wpedantic -O3
)
target_include_directories(
  tree_test
  PRIVATE source
)
target_link_libraries(
  tree_test
  GTest::gtest_main
)

# include(GoogleTest)
# gtest_discover_tests(tree_test)
