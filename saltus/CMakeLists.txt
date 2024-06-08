cmake_minimum_required(VERSION 3.21.2)

# Saltus library
add_library(libsaltus INTERFACE)

add_subdirectory(src)

target_include_directories(libsaltus INTERFACE include)
# Sorry
target_link_options(libsaltus INTERFACE -lxcb -lxcb-ewmh -lxcb-icccm -lvulkan)
