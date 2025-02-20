cmake_minimum_required(VERSION 3.10)

project(Monkey VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

# Define compilation flags for different compilers
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Flags for GCC/Clang
    add_compile_options(
        -Wall # Enable all common warnings
        -Wextra # Enable extra warnings
        -pedantic # Enforce strict compliance with the C++ standard

        # -Werror                 # Treat all warnings as errors
        -Wshadow # Warn on variable shadowing
        -Wnon-virtual-dtor # Warn if a class with virtual functions has no virtual destructor
        -Wold-style-cast # Warn on C-style casts
        -Wcast-align # Warn on potential performance issues with pointer alignment
        -Wunused # Warn on unused variables, functions, etc.
        -Woverloaded-virtual # Warn if a function in a derived class hides a virtual function in a base class
        -Wconversion # Warn on implicit type conversions
        -Wsign-conversion # Warn on signed/unsigned conversions
        -Wformat=2 # Warn on format strings and make warnings more strict
        -Wmissing-declarations # Warn if functions are declared without previous declaration
        -g # Enable debug information for all builds
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # Flags for MSVC
    add_compile_options(
        /W4 # Enable the highest level of warnings in MSVC

        # /WX                    # Treat all warnings as errors
        /permissive- # Enable stricter standards compliance
        /w14640 # Warn on hidden virtual functions
        /w14265 # Warn on unreferenced formal parameters
        /w14242 # Warn on implicit conversions (performance warnings)
        /w14254 # Warn on signed/unsigned mismatches
        /w14263 # Warn on potentially dangerous pointer truncation
        /w14264 # Warn on variable shadowing
        /Zi # Enable debug information
    )
endif()

option(BUILD_TESTS "Build unit tests" ON)

include_directories(include)

set(MONKEY_SRC_FILES
    src/token.cpp
    src/lexer.cpp
    src/repl.cpp
    src/ast.cpp
    src/parser.cpp
    src/object.cpp
    src/evaluator.cpp
    src/globals.cpp
    src/environment.cpp
)

set(MONKEY_HEADER_FILES
    include/token.hpp
    include/lexer.hpp
    include/repl.hpp
    include/ast.hpp
    include/parser.hpp
    include/object.hpp
    include/evaluator.hpp
    include/globals.hpp
    include/environment.hpp
    include/macros.hpp
)

add_executable(monkey main.cpp ${MONKEY_SRC_FILES} ${MONKEY_HEADER_FILES})

# Optionally, build unit tests if enabled
if(BUILD_TESTS)
    # Include FetchContent to download Catch2
    include(FetchContent)

    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0 # Make sure this is the latest stable release
    )

    # Fetch and make Catch2 available
    FetchContent_MakeAvailable(catch2)

    # Explicitly set the source files for the tests
    set(MONKEY_TEST_FILES
        tests/lexer.t.cpp
        tests/parser.t.cpp
        tests/evaluation.t.cpp
    )

    # Add an executable for the tests
    add_executable(monkey_tests ${MONKEY_TEST_FILES} ${MONKEY_SRC_FILES})

    # Link Catch2 with the test executable
    target_link_libraries(monkey_tests PRIVATE Catch2::Catch2WithMain)

    # Automatically discover tests using Catch2
    include(CTest)
    include(Catch)
    catch_discover_tests(monkey_tests)
endif()

# Output build configuration
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")