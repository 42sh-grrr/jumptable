cmake_minimum_required(VERSION 3.21.2)

project(jumptable VERSION 1.0.0 LANGUAGES CXX)

find_library(CRITERION NAMES criterion libcriterion REQUIRED)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS
    "-Wall -Wextra -Werror -pedantic"
)

set(CMAKE_CXX_FLAGS_DEBUG
    "-g3 -fno-inline -fsanitize=address"
)

set(CMAKE_CXX_FLAGS_PROFILING
    "-g3 -fno-inline -O3 -DNDEBUG"
)

set(CMAKE_CXX_FLAGS_RELEASE
    "-O3 -DNDEBUG"
)

set(CMAKE_EXE_LINKER_FLAGS_DEBUG
    "-fsanitize=address"
)

add_subdirectory(logger)
add_subdirectory(matrix)
add_subdirectory(saltus)
add_subdirectory(math)
add_subdirectory(jumptable)
