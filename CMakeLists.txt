cmake_minimum_required(VERSION 3.10)

project(Monkey VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Define build directories
set(BUILD_64, "build_64")
set(BUILD_64REL, "build_64rel")
set(BUILD_64RELWITHDEBINFO, "build_64RelWithDebInfo")

# Ensure CMake does not create a default "build" directory
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# Enable testing
enable_testing()

# Collect source files
include_directories(src)

set(MONKEY_SRC_FILES
    src/Token.cpp
    src/lexer.cpp
    src/repl.cpp
    src/ast.cpp
    src/parser.cpp
    src/object.cpp
    src/evaluator.cpp
    src/globals.cpp
    src/environment.cpp
    src/Token.hpp
    src/lexer.hpp
    src/repl.hpp
    src/ast.hpp
    src/parser.hpp
    src/object.hpp
    src/evaluator.hpp
    src/globals.hpp
    src/environment.hpp
    src/macros.hpp
    src/macros.cpp
    src/SyntaxParser.cpp
    src/SyntaxParser.hpp
    src/TokenTable.cpp
    src/TokenTable.hpp
    src/ASTBuilder.cpp
    src/ASTBuilder.hpp
    src/ASTNode.cpp
    src/ASTNode.hpp
    src/ASTNodeTypes.hpp
    src/VM.hpp
    src/VM.cpp
    src/TSymbolTable.hpp
    src/TSymbolTable.cpp
    src/OpCodes.hpp
    src/OpCodes.cpp
    src/MachineStack.hpp
    src/MachineStack.cpp
    src/ConstantTable.hpp
    src/ConstantTable.cpp
    src/TStringObject.hpp
    src/MemoryManager.hpp
    src/MemoryManager.cpp
    src/TListObject.hpp
    src/TListObject.cpp
    src/TByteCodeBuilder.hpp
    src/TByteCodeBuilder.cpp
)

# Define compilation flags for different compilers
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Flags for GCC/Clang
    add_compile_options(
        -Wall # Enable all common warnings
        -Wextra # Enable extra warnings
        -pedantic # Enforce strict compliance with the C++ standard
        -Wshadow # Warn on variable shadowing
        -Wnon-virtual-dtor # Warn if a class with virtual functions has no virtual destructor
        -Wold-style-cast # Warn on C-style casts
        -Wcast-align # Warn on potential performance issues with pointer alignment
        -Wunused # Warn on unused variables, functions, etc.
        -Woverloaded-virtual # Warn if a function in a derived class hides a virtual function in a base class
        -Wconversion # Warn on implicit type conversions
        -Wformat=2 # Warn on format strings and make warnings more strict
        -Wmissing-declarations # Warn if functions are declared without previous declaration
    )

    # Set Debug optimization level to -O1
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-O0 -g)  # Lower optimization for debugging
    endif()

    # Set Release optimization level to -O3
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -DNDEBUG)  # Full optimization, disable asserts
    endif()
    
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
            tests/program.t.cpp
            tests/vm.t.cpp
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
endif()

option(BUILD_TESTS "Build unit tests" ON)

# Add main executable
add_executable(monkey main.cpp ${MONKEY_SRC_FILES})
add_executable(monkey_benchmarks benchmarks.cpp ${MONKEY_SRC_FILES})

# Output build configuration
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")