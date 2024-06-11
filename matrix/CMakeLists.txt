cmake_minimum_required(VERSION 3.21.2)

# Matrix library
add_library(libmatrix INTERFACE)

target_include_directories(libmatrix INTERFACE include)

# Tests
add_subdirectory(tests)
