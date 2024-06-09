cmake_minimum_required(VERSION 3.21.2)

project(jumptable VERSION 1.0.0 LANGUAGES CXX)

find_library(CRITERION NAMES criterion libcriterion REQUIRED)

add_subdirectory(matrix)
add_subdirectory(jumptable)
