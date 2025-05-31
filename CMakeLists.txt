cmake_minimum_required(VERSION 3.22)

project(
    Monkey
    VERSION 1.0.0
    LANGUAGES C CXX)

# Global CMake variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TEST_NAME "unit_tests")
set(LIBRARY_NAME "Library")
set(REPL_NAME "repl")
set(BENCHMARKS_NAME "Benchmarks")

# Options
option(USE_CPM "Whether to use CPM" ON)
option(ENABLE_WARNINGS "Enable to add warnings to a target." OFF)
option(ENABLE_WARNINGS_AS_ERRORS "Enable to treat warnings as errors." OFF)

option(ENABLE_TESTING "Enable a Unit Testing build." ON)
option(ENABLE_COVERAGE "Enable a Code Coverage build." OFF)
option(ENABLE_CLANG_TIDY "Enable to a clang tidy." ON)

option(ENABLE_SANITIZE_ADDR "Enable address sanitize." OFF)
option(ENABLE_SANITIZE_UNDEF "Enable undefined sanitize." OFF)
option(ENABLE_SANITIZE_LEAK "Enable leak sanitize (Gcc/Clang only)." OFF)
option(ENABLE_SANITIZE_THREAD "Enable thread sanitize (Gcc/Clang only)." OFF)

option(ENABLE_CLANG_FORMAT "Enable to add clang-format." OFF)
option(ENABLE_CMAKE_FORMAT "Enable to add cmake-format." OFF)

option(ENABLE_LTO "Enable to add Link Time Optimization." ON)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

if (ENABLE_SANITIZE_ADDR OR ENABLE_SANITIZE_UNDEF)
    include(Sanitizers)
    add_sanitizer_flags()
endif()

if(ENABLE_LTO)
    include(LTO)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
include(ConfigSafeGuards)
include(Tools)

if(ENABLE_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2
        GIT_TAG v3.5.3
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
endif()

if (ENABLE_TESTING AND ENABLE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif()

add_subdirectory(configured)
add_subdirectory(src)
add_subdirectory(app)

if (ENABLE_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()














# Define build directories
#set(BUILD_64 "build_64")
#set(BUILD_64REL "build_64rel")
#set(BUILD_64RELWITHDEBINFO "build_64RelWithDebInfo")

# Prevent in-source builds
# set(CMAKE_DISABLE_SOURCE_CHANGES ON)
# set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# Enable testing
# enable_testing()
# option(BUILD_TESTS "Build unit tests" ON)

# # Collect source files
# include_directories(src)

# set(MONKEY_SRC_FILES
#     src/Token.cpp
#     src/lexer.cpp
#     src/repl.cpp
#     src/ast.cpp
#     src/parser.cpp
#     src/object.cpp
#     src/evaluator.cpp
#     src/globals.cpp
#     src/environment.cpp
#     src/Token.hpp
#     src/lexer.hpp
#     src/repl.hpp
#     src/ast.hpp
#     src/parser.hpp
#     src/object.hpp
#     src/evaluator.hpp
#     src/globals.hpp
#     src/environment.hpp
#     src/macros.hpp
#     src/macros.cpp
#     src/SyntaxParser.cpp
#     src/SyntaxParser.hpp
#     src/TokenTable.cpp
#     src/TokenTable.hpp
#     src/ASTBuilder.cpp
#     src/ASTBuilder.hpp
#     src/ASTNode.cpp
#     src/ASTNode.hpp
#     src/ASTNodeTypes.hpp
#     src/VM.hpp
#     src/VM.cpp
#     src/TSymbolTable.hpp
#     src/TSymbolTable.cpp
#     src/OpCodes.hpp
#     src/OpCodes.cpp
#     src/MachineStack.hpp
#     src/MachineStack.cpp
#     src/ConstantTable.hpp
#     src/ConstantTable.cpp
#     src/TStringObject.hpp
#     src/MemoryManager.hpp
#     src/MemoryManager.cpp
#     src/TListObject.hpp
#     src/TListObject.cpp
#     src/TByteCodeBuilder.hpp
#     src/TByteCodeBuilder.cpp
# )

# # Compiler settings based on platform
# #if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
# #    # GCC/Clang-specific warnings
# #    add_compile_options(
# #        -Wall
# #        -Wextra
# #        -pedantic
# #        -Wshadow
# #        -Wnon-virtual-dtor
# #        -Wold-style-cast
# #        -Wcast-align
# #        -Wunused
# #        -Woverloaded-virtual
# #        -Wconversion
# #        -Wformat=2
# #        -Wmissing-declarations
# #    )
# #
# #    # Debug optimization level
# #    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
# #        add_compile_options(-O0 -g)
# #    endif()
# #
# #    # Release optimization level
# #    if(CMAKE_BUILD_TYPE STREQUAL "Release")
# #        add_compile_options(-O3 -DNDEBUG)
# #    endif()
# #
# #elseif(MSVC)
# #    # MSVC-specific warnings
# #    add_compile_options(
# #        /W4           # High warning level
# #        /permissive-  # Stricter standards compliance
# #        /MP           # Enable multiprocessing for faster builds
# #        /bigobj       # Prevents object file size limitations
# #    )
# #
# #    # Disable specific warnings
# #    add_compile_options(
# #        /wd4100  # Unused parameters
# #        /wd4201  # Non-standard extension: anonymous structs/unions
# #        /wd4505  # Unused functions
# #    )
# #
# #    # Debug optimization level
# #    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
# #        add_compile_options(/Od /Zi)  # No optimizations, full debugging
# #    endif()
# #
# #    # Release optimization level
# #    if(CMAKE_BUILD_TYPE STREQUAL "Release")
# #        add_compile_options(/O2 /DNDEBUG)  # Optimized build
# #    endif()
# #endif()

# # Optionally, build unit tests if enabled
# if(BUILD_TESTS)
#     # Include FetchContent to download Catch2


#     # Test source files
#     set(MONKEY_TEST_FILES
#         tests/lexer.t.cpp
#         tests/parser.t.cpp
#         tests/evaluation.t.cpp
#         tests/program.t.cpp
#         tests/vm.t.cpp
#     )

#     # Add test executable
#     add_executable(monkey_tests ${MONKEY_TEST_FILES} ${MONKEY_SRC_FILES})

#     # Link Catch2 with the test executable
#     target_link_libraries(monkey_tests PRIVATE Catch2::Catch2WithMain)

#     # Automatically discover tests using Catch2
#     include(CTest)
#     include(Catch)
#     catch_discover_tests(monkey_tests)
# endif()

# # Add main executable
# add_executable(${EXECUTABLE_NAME} main.cpp ${MONKEY_SRC_FILES})
# add_executable(monkey_benchmarks benchmarks.cpp ${MONKEY_SRC_FILES})

# # Output build configuration
# message(STATUS "Project: ${PROJECT_NAME}")
# message(STATUS "Version: ${PROJECT_VERSION}")
# message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
# message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
