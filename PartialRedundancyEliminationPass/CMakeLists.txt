cmake_minimum_required(VERSION 3.22)
project(
    PartialRedundancyEliminationPass
)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_library(PartialRedundancyEliminationPass MODULE
    PartialRedundancyElimination.cpp
)

target_compile_features(PartialRedundancyEliminationPass PRIVATE
    cxx_range_for
    cxx_auto_type
)

set_target_properties(PartialRedundancyEliminationPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)
