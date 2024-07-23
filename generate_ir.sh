#!/bin/bash

# Create directory 'build' if it doesn't exist
if [ ! -d "build" ]; then
  mkdir build
fi

# Navigate to 'build' directory
cd build

# Run CMake and Make for LLVM
cmake ..
make

# Return to the parent directory
cd ..

# Define the source file as a variable
SOURCE_FILE="BasicFunction"

# Run the Clang command
clang -S -emit-llvm -Xclang -disable-O0-optnone examples/$SOURCE_FILE.cpp