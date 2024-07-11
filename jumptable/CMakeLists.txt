cmake_minimum_required(VERSION 3.21.2)

add_executable(jumptable)

add_subdirectory(src)

target_compile_options(jumptable PRIVATE
    "-Wold-style-cast"
)

target_link_libraries(jumptable PRIVATE libmatrix)
target_link_libraries(jumptable PRIVATE libsaltus)
target_link_libraries(jumptable PRIVATE liblogger)
target_link_libraries(jumptable PRIVATE libmath)
