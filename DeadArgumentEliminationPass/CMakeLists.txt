cmake_minimum_required(VERSION 3.22)
project(
    DeadArgumentEliminationPass
)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_library(DeadArgumentEliminationPass MODULE
    DeadArgumentElimination.cpp
)

target_compile_features(DeadArgumentEliminationPass PRIVATE 
    cxx_range_for 
    cxx_auto_type
)

set_target_properties(DeadArgumentEliminationPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)
