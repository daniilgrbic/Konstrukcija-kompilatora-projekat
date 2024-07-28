#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ]; then
  echo "Usage: $0 FOLDER_NAME SOURCE_FILE.cpp"
  exit 1
fi

FOLDER_NAME="$1"
SOURCE_FILE="$2"
if [ "$1" == "DeadArgumentEliminationPass" ]; then
    OPT_ARG="dead-arg-elim"
elif [ "$1" == "AlwaysInlinePass" ]; then
    OPT_ARG="always-inline-pass"
else
    OPT_ARG=""
fi

cd "${FOLDER_NAME}"

if [ ! -d "build" ]; then
  mkdir build
fi

cd build
cmake ..
make
cd ..

clang -S -emit-llvm -Xclang -disable-llvm-passes "examples/${SOURCE_FILE}.cpp" -o "${SOURCE_FILE}.ll"
opt -enable-new-pm=0 -load ./build/lib${FOLDER_NAME}.so -"${OPT_ARG}" -S "${SOURCE_FILE}.ll" -o "${SOURCE_FILE}_opt.ll" 

