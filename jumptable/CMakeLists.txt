cmake_minimum_required(VERSION 3.21.2)

add_executable(jumptable)

add_subdirectory(src)

target_compile_options(jumptable PRIVATE
    "-Wall"
    "-Wextra"
    "-Werror"
    "-Wold-style-cast"
    "-pedantic"
)

target_link_libraries(jumptable PRIVATE libmatrix)

set_target_properties(jumptable PROPERTIES
    CXX_STANDARD 20
)
