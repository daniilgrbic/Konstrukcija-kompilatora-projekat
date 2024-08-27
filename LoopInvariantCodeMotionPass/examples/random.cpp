#include <iostream>

void foo(int *arr, int n, int a, int b) {
    int c = a * b;

    for (int i = 0; i < n; ++i) {
        arr[i] = arr[i] + c;
    }
}

int main() {
    int arr[5] = {1, 2, 3, 4, 5};
    foo(arr, 5, 3, 4);

    for (int i = 0; i < 5; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}