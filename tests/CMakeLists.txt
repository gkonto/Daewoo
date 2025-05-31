include(Catch)

set(TEST_SOURCES
    evaluation.t.cpp
    lexer.t.cpp
    parser.t.cpp
    program.t.cpp
    vm.t.cpp
)
set(TEST_INCLUDES "./")

add_executable(${TEST_NAME} ${TEST_SOURCES})
target_include_directories(${TEST_NAME} PUBLIC ${TEST_INCLUDES})
target_link_libraries(${TEST_NAME} PUBLIC Library Catch2::Catch2WithMain)

catch_discover_tests(${TEST_NAME})

if(${ENABLE_LTO})
    target_enable_lto(
        TARGET
        ${TEST_NAME}
        ENABLE
        ON)
endif()

if (${ENABLE_CLANG_TIDY})
    add_clang_tidy_to_target(${TEST_NAME})
endif()
