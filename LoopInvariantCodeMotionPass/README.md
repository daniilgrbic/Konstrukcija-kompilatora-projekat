# Loop-Invariant Code Motion (LICM) Pass

This LLVM pass performs **Loop-Invariant Code Motion (LICM)**, which optimizes loops by moving computations that remain constant throughout the loop (i.e., loop-invariant code) outside the loop body. This reduces redundant computations inside the loop, improving performance.

## Overview

The `LICMPass` identifies instructions that remain invariant throughout the loop and hoists them to the preheader (a basic block that dominates the loop entry).

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
    ...
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
  ...
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