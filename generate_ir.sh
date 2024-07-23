#!/bin/bash

# Check if the source file argument is provided
if [ -z "$1" ]; then
  echo "Usage: $0 SOURCE_FILE.cpp"
  exit 1
fi

# Get the source file from the argument
SOURCE_FILE="$1"

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

# Run the Clang command
clang -S -emit-llvm -Xclang -disable-O0-optnone "examples/$SOURCE_FILE"
