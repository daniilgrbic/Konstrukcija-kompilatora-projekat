# Always Inline Pass

Inlines function calls with _always_inline_ attribute
<br>
Consider following example:
```cpp
inline void sayHello() __attribute__((always_inline));

inline void sayHello() {
    int a = 5;
    int b = 6;
    int c = a + b;
}

void greet() {
    sayHello();
}

int main() {
    greet();
    return 0;
}
```

`greet()` function is calling `sayHello()`, which this optimization inlines because of the attribute defined. Because of this, `greet` function (in C++) should look like following:

```cpp
void greet() {
    int a = 5;
    int b = 6;
    int c = a + b;
}
```

Let's look at the generated IR code for the `greet()` method:

<table>
    <tr>
        <th>Before</th>
        <th>After</th>
    </tr>
    <tr>
        <td>
            <pre>
<code>

; Function Attrs: mustprogress noinline optnone uwtable
define dso_local void @_Z5greetv() #2 {
  call void @_Z8sayHellov()
  ret void
}

; Function Attrs: alwaysinline mustprogress nounwind uwtable
define linkonce_odr dso_local void @_Z8sayHellov() #3 comdat {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 5, i32* %1, align 4
  store i32 6, i32* %2, align 4
  %4 = load i32, i32* %1, align 4
  %5 = load i32, i32* %2, align 4
  %6 = add nsw i32 %4, %5
  store i32 %6, i32* %3, align 4
  ret void
}

</code>
            </pre>
        </td>
        <td>
            <pre>
<code>
; Function Attrs: mustprogress noinline optnone uwtable
define dso_local void @_Z5greetv() #2 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 5, i32* %1, align 4
  store i32 6, i32* %2, align 4
  %7 = load i32, i32* %1, align 4
  %8 = load i32, i32* %2, align 4
  %9 = add nsw i32 %7, %8
  store i32 %9, i32* %3, align 4
  ret void
}

; Function Attrs: alwaysinline mustprogress nounwind uwtable
define linkonce_odr dso_local void @_Z8sayHellov() #3 comdat {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 5, i32* %1, align 4
  store i32 6, i32* %2, align 4
  %4 = load i32, i32* %1, align 4
  %5 = load i32, i32* %2, align 4
  %6 = add nsw i32 %4, %5
  store i32 %6, i32* %3, align 4
  ret void
}

</code>
            </pre>
        </td>
    </tr>
</table>

We can see that greet function doesn't call sayHello() and instead inlines the parameters.