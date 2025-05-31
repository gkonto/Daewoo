include(Catch)

set(TEST_SOURCES
    evaluation.t.cpp
    lexer.t.cpp
    parser.t.cpp
    program.t.cpp
    vm.t.cpp)
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

if(${ENABLE_CLANG_TIDY})
    add_clang_tidy_to_target(${TEST_NAME})
endif()

if(ENABLE_COVERAGE)
    set(COVERAGE_MAIN "coverage")
    set(COVERAGE_EXCLUDES
        "${PROJECT_SOURCE_DIR}/app/*"
        "${PROJECT_SOURCE_DIR}/cmake/*"
        "${PROJECT_SOURCE_DIR}/docs/*"
        "${PROJECT_SOURCE_DIR}/external/*"
        "${PROJECT_SOURCE_DIR}/tests/*"
        "${PROJECT_SOURCE_DIR}/build/*"
        "/usr/include/*")
    set(COVERAGE_EXTRA_FLAGS)
    set(COVERAGE_DEPENDENCIES ${TEST_NAME})

    setup_target_for_coverage_gcovr_html(
        NAME
        ${COVERAGE_MAIN}
        EXECUTABLE
        ctest
        DEPENDENCIES
        ${COVERAGE_DEPENDENCIES}
        BASE_DIRECTORY
        ${CMAKE_SOURCE_DIR})
endif()
