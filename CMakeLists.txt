cmake_minimum_required(VERSION 3.14)
project(myopta)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")

add_library(libmyopta STATIC
  src/ga.cc
  src/crossover.cc
  src/evaluator.cc
  src/problem.cc
)
set_target_properties(libmyopta PROPERTIES OUTPUT_NAME "myopta")
target_include_directories(libmyopta
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Tests
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Unit tests
add_executable(
  test_pool
  test/pool.cc
)
target_link_libraries(
  test_pool
  PRIVATE libmyopta
  GTest::gtest_main
)
add_executable(
  test_misc
  test/misc.cc
)
target_link_libraries(
  test_misc
  PRIVATE libmyopta
  GTest::gtest_main
)

add_executable(
  test_problem
  test/problem.cc
)
target_link_libraries(
  test_problem
  PRIVATE libmyopta
  GTest::gtest_main
)

add_executable(
  test_crossover
  test/crossover.cc
)
target_link_libraries(
  test_crossover
  PRIVATE libmyopta
  GTest::gtest_main
)

add_executable(
  test_evaluator
  test/evaluator.cc
)
target_link_libraries(
  test_evaluator
  PRIVATE libmyopta
  GTest::gtest_main
)

add_executable(
  test_ga
  test/ga.cc
)
target_link_libraries(
  test_ga
  PRIVATE libmyopta
  GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(test_pool)
gtest_discover_tests(test_problem)
gtest_discover_tests(test_crossover)
gtest_discover_tests(test_evaluator)
gtest_discover_tests(test_ga)
