# Loop-Invariant Code Motion (LICM) Pass

This LLVM pass performs **Loop-Invariant Code Motion (LICM)**, which optimizes loops by moving computations that remain constant throughout the loop (i.e., loop-invariant code) outside the loop body. This reduces redundant computations inside the loop, improving performance.

## Overview

The `LICMPass` identifies instructions that remain invariant throughout the loop and hoists them to the preheader (a basic block that dominates the loop entry). In this implementation, the pass specifically targets instructions like `AddOperator` (addition operations) and checks whether their operands change inside the loop. If an operand does not change, it is considered loop-invariant and is moved outside the loop.

### Key Features

- **Loop-Invariant Detection:** The pass checks whether the operands of an addition operation change within the loop. If they don't, the instruction and its operands are hoisted to the preheader.
- **Instruction Hoisting:** Invariant instructions are moved to the loop preheader to avoid redundant execution within the loop.

### How the Pass Works

1. **Check Loop Invariant Conditions:**
   The pass iterates through the instructions in the loop and checks whether an instruction is an addition operation (`AddOperator`). It checks if the operands of the instruction change during the loop execution.

2. **Hoist Invariant Instructions:**
   If both operands are invariant, the instruction and its operands are moved to the preheader block, just before the loop begins.

### Example


```c++
    for(int i = 0; i < c1 + c2; i++) {
        r = r + r;
    }

    c1 + c2 is moved to the preheader
```

<table>
    <tr>
        <th>Before</th>
        <th>After</th>
    </tr>
    <tr>
        <td>
            <pre>
<code>
define dso_local i32 @main() #4 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 5, i32* %2, align 4
  store i32 1, i32* %3, align 4
  store i32 3, i32* %4, align 4
  store i32 2, i32* %5, align 4
  store i32 0, i32* %6, align 4
  br label %7

7:                                                ; preds = %17, %0
  %8 = load i32, i32* %6, align 4
  %9 = load i32, i32* %4, align 4
  %10 = load i32, i32* %5, align 4
  %11 = add nsw i32 %9, %10
  %12 = icmp slt i32 %8, %11
  br i1 %12, label %13, label %20

</code>
            </pre>
        </td>
        <td>
            <pre>
<code>
define dso_local i32 @main() #4 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 5, i32* %2, align 4
  store i32 1, i32* %3, align 4
  store i32 3, i32* %4, align 4
  store i32 2, i32* %5, align 4
  store i32 0, i32* %6, align 4
  %7 = load i32, i32* %4, align 4
  %8 = load i32, i32* %5, align 4
  %9 = add nsw i32 %7, %8
  br label %10

10:                                               ; preds = %17, %0
  %11 = load i32, i32* %6, align 4
  %12 = icmp slt i32 %11, %9
  br i1 %12, label %13, label %20
</code>
            </pre>
        </td>
    </tr>
</table>