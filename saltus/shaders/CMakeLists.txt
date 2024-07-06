cmake_minimum_required(VERSION 3.21.2)

add_custom_target(shaders
    COMMENT "Building shaders..."
    COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/shader.frag -o ${CMAKE_CURRENT_BINARY_DIR}/shader.frag.spv
    COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/shader.vert -o ${CMAKE_CURRENT_BINARY_DIR}/shader.vert.spv
)
