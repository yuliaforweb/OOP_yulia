#include <iostream>

using namespace std;

int multiply(int a, int b) {
    return a * b;
}

int multiply(int a, int b, int c) {
    return a * b * c;
}

int main() {
    cout << "Добуток двох чисел: " << multiply(3, 3) << endl;
    cout << "Добуток трьох чисел: " << multiply(5, 4, 0) << endl;

    return 0;
}
