cmake_minimum_required(VERSION 3.16)

# Find testing framework
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    # Download and build Google Test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50f33f9142fff8c5ecd7e20b966cdf.zip
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Test sources
set(TEST_SOURCES
    test_lexer.cpp
    test_parser.cpp
    test_codegen.cpp
    test_integration.cpp
)

# Create test executable
add_executable(sdl_compiler_tests ${TEST_SOURCES})

target_link_libraries(sdl_compiler_tests
    sdl_compiler_lib
    gtest_main
    gtest
)

target_include_directories(sdl_compiler_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(sdl_compiler_tests)
