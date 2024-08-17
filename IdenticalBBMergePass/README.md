# Identical (Duplicate) Basic Block Merge

Merges identical basic blocks. 

Checks are needed to determine safe-to-merge blocks. For example, only
identical blocks which share a common successor and both end with an
unconditional jump are considered for merging. 

We consider two blocks BB1 and BB2 identical if all instructions in BB1 are
identical to those in BB2. In order to check this, we create a mapping of
function result values of known identical functions, and consider them in the
future when comparing instructions later.

A trivial example of a program this pass would optimize is an if...else
statement with identical code in both paths. See the example below:

<table>
    <tr>
        <th>Before</th>
        <th>After</th>
    </tr>
    <tr>
        <td>
            <pre>
<code>
    br i1 %8, label %9, label %13
                        ;
9:    ; preds = %2
    %10 = load i32, i32* %3, align 4
    %11 = load i32, i32* %4, align 4
    %12 = add nsw i32 %10, %11
    store i32 %12, i32* %5, align 4
    br label %17  
                        ; 
13:    ; preds = %2
    %14 = load i32, i32* %3, align 4
    %15 = load i32, i32* %4, align 4
    %16 = add nsw i32 %14, %15
    store i32 %16, i32* %5, align 4
    br label %17
                        ; 
17:    ; preds = %13, %9
    %18 = load i32, i32* %5, align 4
    ret i32 %18
</code>
            </pre>
        </td>
        <td>
            <pre>
<code>
    br i1 %8, label %9, label %9
                        ; 
9:    ; preds = %2, %2
    %10 = load i32, i32* %3, align 4
    %11 = load i32, i32* %4, align 4
    %12 = add nsw i32 %10, %11
    store i32 %12, i32* %5, align 4
    br label %13
                        ; 
13:   ; preds = %9
    %14 = load i32, i32* %5, align 4
    ret i32 %14
</code>
            </pre>
        </td>
    </tr>
</table>