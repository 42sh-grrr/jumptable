cmake_minimum_required(VERSION 3.21.2)

# Saltus library
add_library(libsaltus STATIC)

set_target_properties(libsaltus PROPERTIES
    CXX_STANDARD 20
    OUTPUT_NAME saltus
)

add_subdirectory(shaders)
add_subdirectory(src)

target_compile_options(libsaltus PRIVATE
    "-Wall"
    "-Wextra"
    "-Werror"
    "-Wold-style-cast"
    "-pedantic"
    # "-g3"
    # "-fsanitize=address"
)
target_link_options(libsaltus PRIVATE
    # "-fsanitize=address"
)

add_dependencies(libsaltus
    shaders
)

find_library(VULKAN NAMES vulkan libvulkan REQUIRED)
find_library(XCB NAMES xcb libxcb Xcb REQUIRED)
find_library(XCB_ICCCM NAMES xcb-icccm libxcb-icccm REQUIRED)

target_include_directories(libsaltus PUBLIC include)
target_link_libraries(libsaltus PRIVATE libmatrix)
target_link_libraries(libsaltus PRIVATE ${VULKAN})
target_link_libraries(libsaltus PRIVATE ${XCB})
target_link_libraries(libsaltus PRIVATE ${XCB_ICCCM})
