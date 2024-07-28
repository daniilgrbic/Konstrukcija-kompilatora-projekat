#include <iostream>

int should_delete(int x) {
    return 1;
}

int main() {
    int x = 0;
    int y = should_delete(x);
    std::cout << y << std::endl;
    return 0;
}