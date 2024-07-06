cmake_minimum_required(VERSION 3.21.2)

add_executable(test_libmath)

target_sources(test_libmath PRIVATE
    test_libmath.cc
)

target_compile_options(test_libmath PRIVATE
    "-Wall"
    "-Wextra"
    "-Werror"
    "-pedantic"
)

target_link_libraries(test_libmath PRIVATE ${CRITERION})
target_link_libraries(test_libmath PRIVATE libmath)
target_link_libraries(test_libmath PRIVATE libmatrix)

set_target_properties(test_libmath PROPERTIES
    CXX_STANDARD 20
)


add_custom_command(TARGET test_libmath POST_BUILD
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_libmath
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running libmath check..."
)

