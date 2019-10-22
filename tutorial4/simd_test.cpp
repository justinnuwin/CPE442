#include <iostream>

int main(void) {
    int x[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int y[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int sum = 0;
    int i;
#pragma omp simd reduction(+:sum)
    for (i = 0; i < 11; i++) {
        sum = x[i] + y[i];
    }
    std::cout << sum << std::endl;
    return sum;
}
