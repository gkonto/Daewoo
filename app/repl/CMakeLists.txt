set(LIBRARY_INCLUDES
    "./"
    "${CMAKE_BINARY_DIR}/configured_files/include")

add_executable(Executable main.cpp)
target_link_libraries(Executable PUBLIC Library)
target_include_directories(Executable PUBLIC 
    ${LIBRARY_INCLUDES}
)

if(${ENABLE_LTO})
    target_enable_lto(
        TARGET
        Executable
        ENABLE
        ON)
endif()