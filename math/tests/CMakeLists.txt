cmake_minimum_required(VERSION 3.21.2)

add_executable(test_libmath)

target_sources(test_libmath PRIVATE
    test_libmath.cc
)

target_link_libraries(test_libmath PRIVATE ${CRITERION})
target_link_libraries(test_libmath PRIVATE libmath)
target_link_libraries(test_libmath PRIVATE libmatrix)

add_custom_command(TARGET test_libmath POST_BUILD
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_libmath
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running libmath check..."
)

