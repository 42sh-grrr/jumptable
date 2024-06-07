cmake_minimum_required(VERSION 3.21.2)

# Graphics library
add_library(libgraphics INTERFACE)

add_subdirectory(src)

target_include_directories(libgraphics INTERFACE include)
# Sorry
target_link_options(libgraphics INTERFACE -lxcb -lxcb-ewmh -lxcb-icccm)
