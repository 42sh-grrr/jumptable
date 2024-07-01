cmake_minimum_required(VERSION 3.21.2)

add_library(liblogger SHARED)

add_subdirectory(src)

set_target_properties(liblogger PROPERTIES CXX_STANDARD 20 OUTPUT_NAME logger)

target_include_directories(liblogger PUBLIC include)
target_include_directories(liblogger PRIVATE src)
