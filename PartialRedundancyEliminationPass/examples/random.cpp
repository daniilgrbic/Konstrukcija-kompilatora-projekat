#include <iostream>

int compute(int x, int y, int z) {
    int result = 0;

    if (x > 0) {
        result = (x * y) + z; // x * y is computed here
    }

    if (x <= 0 || z > 10) {
        result = (x * y) - z; // x * y is computed again
    }

    return result;
}