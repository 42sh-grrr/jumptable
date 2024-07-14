cmake_minimum_required(VERSION 3.21.2)

# Saltus library
add_library(libsaltus STATIC)

set_target_properties(libsaltus PROPERTIES
    OUTPUT_NAME saltus
)

add_subdirectory(shaders)
add_subdirectory(src)

# Tests
add_subdirectory(tests)

target_compile_options(libsaltus PRIVATE
    "-Wold-style-cast"
)

add_dependencies(libsaltus
    shaders
)

find_library(VULKAN NAMES vulkan libvulkan REQUIRED)
find_library(XCB NAMES xcb libxcb Xcb REQUIRED)
find_library(XCB_ICCCM NAMES xcb-icccm libxcb-icccm REQUIRED)
find_library(XCB_FIXES NAMES xcb-xfixes libxcb-xfixes REQUIRED)

target_include_directories(libsaltus PUBLIC include)
target_include_directories(libsaltus PRIVATE src)

target_link_libraries(libsaltus PUBLIC libmatrix)
target_link_libraries(libsaltus PUBLIC liblogger)
target_link_libraries(libsaltus PRIVATE ${VULKAN})
target_link_libraries(libsaltus PRIVATE ${XCB})
target_link_libraries(libsaltus PRIVATE ${XCB_ICCCM})
target_link_libraries(libsaltus PRIVATE ${XCB_FIXES})
