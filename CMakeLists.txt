cmake_minimum_required(VERSION 3.22)
project(
    AlwaysInlineFuncInlinerPass
)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_library(AIFIPass MODULE
    AlwaysInlineFuncInlinerPass.cpp
)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(AIFIPass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(AIFIPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)
