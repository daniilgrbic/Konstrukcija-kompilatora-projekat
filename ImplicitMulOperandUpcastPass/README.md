# Implicit MulOperand Upcast Pass

This is a joke pass which stemmed from my frustration after forgetting to 
upcast `int` operands in a multiplication that went into a `long long` during
a [CodeForces](https://codeforces.com) round (on multiple occasions).

The pass does one and only one thing - when it sees a multiplication of
32-bit integers being saved in a 64-bit container without an explicit
conversion, it implicitly converts the operands. 

So, the code
```c++
int a=..., b=...;
long long c = a * b;
```
becomes
```c++
int a=..., b=...;
long long c = (long long) a * b;
```

Here is how this looks in LLVM IR:

<table>
    <tr>
        <th>Before</th>
        <th>After</th>
    </tr>
    <tr>
        <td>
            <pre>
<code>
%a_32 = load i32, i32* ..., align 4
%b_32 = load i32, i32* ..., align 4
%tmp_32 = mul nsw i32 %a_32, %b_32
%c_64 = sext i32 %tmp_32 to i64
</code>
            </pre>
        </td>
        <td>
            <pre>
<code>
%a_32 = load i32, i32* ..., align 4
%b_32 = load i32, i32* ..., align 4
%a_64 = sext i32 %a_32 to i64
%b_64 = sext i32 %b_32 to i64
%c_64 = mul i64 %a_64, %b_64
%dead1 = mul nsw i32 %a_32, %b_32
%dead2 = sext i32 %dead1 to i64
</code>
            </pre>
        </td>
    </tr>
</table>