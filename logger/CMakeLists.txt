cmake_minimum_required(VERSION 3.21.2)

add_library(liblogger SHARED)

# Logger library
add_subdirectory(src)

set_target_properties(liblogger PROPERTIES
    OUTPUT_NAME logger
)

target_compile_options(liblogger PRIVATE
    "-Wold-style-cast"
)

target_include_directories(liblogger PUBLIC include)
target_include_directories(liblogger PRIVATE src)

# Tests
add_subdirectory(tests)
