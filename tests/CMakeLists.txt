cmake_minimum_required(VERSION 3.16)
project(buggy_calculator_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Test)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(CALC_SOURCES
  ../calculator.cpp
  ../button.cpp
)

# Caja negra: sin BUGGY_CALCULATOR_TESTING (no friend, no acceso a privados).
add_executable(buggy_calc_blackbox_tests
  blackbox_main.cpp
  test_blackbox_comprehensive.cpp
  ${CALC_SOURCES}
)

target_include_directories(buggy_calc_blackbox_tests PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_link_libraries(buggy_calc_blackbox_tests PRIVATE
  Qt5::Widgets
  Qt5::Test
  GTest::gtest
)

# Caja blanca: incluye la misma batería observable que caja negra + pruebas estructurales.
add_executable(buggy_calc_whitebox_tests
  whitebox_main.cpp
  test_blackbox_comprehensive.cpp
  test_whitebox_comprehensive.cpp
  ${CALC_SOURCES}
)

target_include_directories(buggy_calc_whitebox_tests PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_compile_definitions(buggy_calc_whitebox_tests PRIVATE BUGGY_CALCULATOR_TESTING)

target_link_libraries(buggy_calc_whitebox_tests PRIVATE
  Qt5::Widgets
  Qt5::Test
  GTest::gtest
)

option(ENABLE_COVERAGE "Add GCC/Clang coverage flags (Linux/macOS)" OFF)
if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  foreach(t buggy_calc_blackbox_tests buggy_calc_whitebox_tests)
    target_compile_options(${t} PRIVATE --coverage -O0 -g)
    target_link_options(${t} PRIVATE --coverage)
  endforeach()
endif()

add_test(NAME buggy_calc_blackbox COMMAND buggy_calc_blackbox_tests)
add_test(NAME buggy_calc_whitebox COMMAND buggy_calc_whitebox_tests)
if(UNIX)
  set_tests_properties(buggy_calc_blackbox buggy_calc_whitebox PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()
