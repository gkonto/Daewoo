add_executable(${BENCHMARKS_NAME} benchmarks.cpp)
target_link_libraries(${BENCHMARKS_NAME} PUBLIC ${LIBRARY_NAME})

if(${ENABLE_LTO})
    target_enable_lto(
        TARGET
        ${BENCHMARKS_NAME}
        ENABLE
        ON)
endif()
