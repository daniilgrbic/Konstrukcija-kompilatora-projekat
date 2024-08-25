cmake_minimum_required(VERSION 3.22)
project(
    ImplicitMulOperandUpcastPass
)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_library(ImplicitMulOperandUpcastPass MODULE
    ImplicitMulOperandUpcast.cpp
)

target_compile_features(ImplicitMulOperandUpcastPass PRIVATE 
    cxx_range_for 
    cxx_auto_type
)

set_target_properties(ImplicitMulOperandUpcastPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)
