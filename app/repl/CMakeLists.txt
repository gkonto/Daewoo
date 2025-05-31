set(LIBRARY_INCLUDES
    "./"
    "${CMAKE_BINARY_DIR}/configured_files/include")

add_executable(${REPL_NAME} main.cpp)
target_link_libraries(${REPL_NAME} PUBLIC Library)
target_include_directories(${REPL_NAME} PUBLIC 
    ${LIBRARY_INCLUDES}
)

if(${ENABLE_LTO})
    target_enable_lto(
        TARGET
        ${REPL_NAME}
        ENABLE
        ON)
endif()