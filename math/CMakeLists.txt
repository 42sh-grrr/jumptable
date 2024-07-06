cmake_minimum_required(VERSION 3.21.2)

add_library(libmath SHARED)

# Math library
add_subdirectory(src)

set_target_properties(libmath PROPERTIES
    CXX_STANDARD 20
    OUTPUT_NAME math
)

target_link_libraries(libmath PRIVATE libmatrix)

target_include_directories(libmath PUBLIC include)
target_include_directories(libmath PRIVATE src)

# Tests
add_subdirectory(tests)
