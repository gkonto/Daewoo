include(Catch)

set(TEST_MAIN "unit_tests")
set(TEST_SOURCES
    evaluation.t.cpp
    lexer.t.cpp
    parser.t.cpp
    program.t.cpp
    vm.t.cpp
)
set(TEST_INCLUDES "./")

add_executable(${TEST_MAIN} ${TEST_SOURCES})
target_include_directories(${TEST_MAIN} PUBLIC ${TEST_INCLUDES})
target_link_libraries(${TEST_MAIN} PUBLIC Library Catch2::Catch2WithMain)

catch_discover_tests(${TEST_MAIN})