cmake_minimum_required(VERSION 3.21.2)

target_sources(libgraphics INTERFACE
    graphics.cc
    window.cc
    window_events.cc
)
