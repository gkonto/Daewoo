
add_executable(Benchmarks benchmarks.cpp)
target_link_libraries(Benchmarks PUBLIC Library)

if(${ENABLE_LTO})
    target_enable_lto(
        TARGET
        Benchmarks
        ENABLE
        ON)
endif()