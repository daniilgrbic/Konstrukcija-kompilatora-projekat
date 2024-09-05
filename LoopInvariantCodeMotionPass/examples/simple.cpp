#include <iostream>

int main() {

    int n = 5;
    int r = 1;
    int c1 = 3;
    int c2 = 2;

    for(int i = 0; i < c1 + c2; i++) {
        r = r + r;
    }

    std::cout << r << std::endl;

    return 0;
}