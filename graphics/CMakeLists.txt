cmake_minimum_required(VERSION 3.21.2)

# Graphics library
add_library(libgraphics INTERFACE)

add_subdirectory(src)

find_package(X11)

target_include_directories(libgraphics INTERFACE include)
target_link_libraries(libgraphics INTERFACE X11)
