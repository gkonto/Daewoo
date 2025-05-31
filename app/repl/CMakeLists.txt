
add_executable(Executable main.cpp)
target_link_libraries(Executable PUBLIC Library)
target_include_directories(Executable PUBLIC 
    "./"
    "${CMAKE_BINARY_DIR}/configured_files/include"
)