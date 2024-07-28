#include <iostream>

inline void sayHello() __attribute__((always_inline));

inline void sayHello() {
    std::cout << "Hello, LLVM!" << std::endl;
}

void greet() {
    sayHello();
}

int main() {
    greet();
    return 0;
}
