cmake_minimum_required(VERSION 3.21.2)

target_sources(libsaltus INTERFACE
    window.cc
    window_events.cc
    renderer.cc
    vulkan/vulkan_renderer.cc
)
