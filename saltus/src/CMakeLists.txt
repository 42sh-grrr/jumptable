cmake_minimum_required(VERSION 3.21.2)

FILE(GLOB SOURCES *.cc vulkan/*.cc)
target_sources(libsaltus PRIVATE ${SOURCES})
