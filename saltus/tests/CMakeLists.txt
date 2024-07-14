cmake_minimum_required(VERSION 3.21.2)

add_executable(test_libsaltus)

FILE(GLOB SOURCES *.cc)
target_sources(test_libsaltus PRIVATE ${SOURCES})

target_link_libraries(test_libsaltus PRIVATE ${CRITERION})
target_link_libraries(test_libsaltus PRIVATE libsaltus)

add_custom_command(TARGET test_libsaltus POST_BUILD
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_libsaltus
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running libsaltus check..."
)
