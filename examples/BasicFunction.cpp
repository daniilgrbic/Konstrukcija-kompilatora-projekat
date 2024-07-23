#include <iostream>

int f() {
    if (true) {
        return 1;
    }
    else {
        return -1;
    }
}

int main() {
    int a = f();

    std::cout << a << "\n";
    return 0;
}