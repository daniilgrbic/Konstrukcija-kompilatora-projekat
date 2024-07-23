#include <iostream>

inline void always_inline_func() __attribute__((always_inline));

inline void always_inline_func() {
    std::cout << "Always inlined function!" << std::endl;
}

void caller() {
    always_inline_func();
}

int main() {
    caller();
    return 0;
}
