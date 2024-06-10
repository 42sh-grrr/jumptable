cmake_minimum_required(VERSION 3.21.2)

# Saltus library
add_library(libsaltus STATIC)

add_subdirectory(src)

target_include_directories(libsaltus PUBLIC include)
# Sorry
target_link_options(libsaltus PUBLIC -lxcb -lxcb-ewmh -lxcb-icccm -lvulkan)
