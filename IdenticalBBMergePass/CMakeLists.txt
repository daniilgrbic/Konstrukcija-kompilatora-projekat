cmake_minimum_required(VERSION 3.22)
project(
    IdenticalBBMergePass
)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_library(IdenticalBBMergePass MODULE
    IdenticalBBMerge.cpp
)

target_compile_features(IdenticalBBMergePass PRIVATE 
    cxx_range_for 
    cxx_auto_type
)

set_target_properties(IdenticalBBMergePass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)
