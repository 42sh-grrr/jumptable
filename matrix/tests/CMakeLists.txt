cmake_minimum_required(VERSION 3.21.2)

add_executable(test_libmatrix)

target_sources(test_libmatrix PRIVATE
    test_libmatrix.cc
)

target_compile_options(test_libmatrix PRIVATE
    "-Wall"
    "-Wextra"
    "-Werror"
    "-pedantic"
)

target_link_libraries(test_libmatrix PRIVATE ${CRITERION})
target_link_libraries(test_libmatrix PRIVATE libmatrix)

set_target_properties(test_libmatrix PROPERTIES
    CXX_STANDARD 20
)


add_custom_command(TARGET test_libmatrix POST_BUILD
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_libmatrix
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running libmatrix check..."
)
