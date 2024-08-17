#!/bin/bash

cYellow='\033[0;33m'
cOff='\033[0m'
cGreen='\033[0;32m'
mInfo="${cOff}[${cGreen}INFO${cOff}]"

llvmPasses=($(find "$(pwd)" -maxdepth 1 -type d -name "*Pass"))

echo "Choose LLVM pass:"
for i in "${!llvmPasses[@]}"; do
    echo "  $((i + 1)) - $(basename "${llvmPasses[$i]}")"
done

read -p "Enter number from 1 to ${#llvmPasses[@]}: " i
selectedPass=${llvmPasses[$((i - 1))]}

selectedPass=$(basename $selectedPass)
echo -e "$mInfo Selected pass: $selectedPass"

# Enter the selected pass subfolder
cd "$selectedPass"
passArg=$(< passArg)
echo -e "$mInfo Selected pass argument: $passArg"

# Build the LLVM pass using CMake
if [ ! -d "build" ]; then
  mkdir build
fi
cd build
cmake ..
make
cd ..

# Generate the IR for examples
cd examples
for file in *.cpp; do
  if [[ -f "$file" ]]; then
    echo -e "$mInfo Generating IR for $file"
    clang -S -emit-llvm -Xclang -disable-llvm-passes "$file" -o "$file.unopt.ll"
    echo -e "$mInfo Optimizing $file.unopt.ll"
    echo -ne "${cYellow}"
    opt -enable-new-pm=0 -load "../build/lib$selectedPass.so" -"$passArg" -S "$file.unopt.ll" -o "$file.opt.ll" 
    echo -e "$mInfo ${cGreen}DONE! See $file.opt.ll${cOff}"
  fi
done
cd ..

# Finally, return to root folder
cd ..
