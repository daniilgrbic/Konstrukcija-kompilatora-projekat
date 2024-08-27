cmake_minimum_required(VERSION 3.22)
project(
    LoopInvariantCodeMotionPass
)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_library(LoopInvariantCodeMotionPass MODULE
    LoopInvariantCodeMotion.cpp
)

target_compile_features(LoopInvariantCodeMotionPass PRIVATE
    cxx_range_for
    cxx_auto_type
)

set_target_properties(LoopInvariantCodeMotionPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)
