cmake_minimum_required(VERSION 3.16)
project(SDL_Shader_Compiler VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Find required packages
find_package(PkgConfig QUIET)

# Source files
set(CORE_SOURCES
    src/lexer/lexer.cpp
    src/lexer/token.cpp
    src/parser/parser.cpp
    src/parser/ast.cpp
    src/semantic/analyzer.cpp
    src/semantic/symbol_table.cpp
    src/codegen/glsl_generator.cpp
    src/codegen/cuda_generator.cpp
    src/codegen/base_generator.cpp
    src/compiler/compiler.cpp
    src/utils/error_handler.cpp
    src/utils/file_utils.cpp
)

set(CLI_SOURCES
    src/main.cpp
    src/cli/cli_parser.cpp
)

# Create main compiler library
add_library(sdl_compiler_lib STATIC ${CORE_SOURCES})

# Create main executable
add_executable(sdl_compiler ${CLI_SOURCES})
target_link_libraries(sdl_compiler sdl_compiler_lib)

# Testing (optional)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install targets
install(TARGETS sdl_compiler DESTINATION bin)

# Custom targets for building different configurations
add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
    COMMENT "Building debug configuration"
)

add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
    COMMENT "Building release configuration"
)
