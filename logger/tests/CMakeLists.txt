cmake_minimum_required(VERSION 3.21.2)

add_executable(test_liblogger)

target_sources(test_liblogger PRIVATE
    test_liblogger.cc
)

target_link_libraries(test_liblogger PRIVATE ${CRITERION})
target_link_libraries(test_liblogger PRIVATE liblogger)

add_custom_command(TARGET test_liblogger POST_BUILD
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_liblogger
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running liblogger check..."
)
