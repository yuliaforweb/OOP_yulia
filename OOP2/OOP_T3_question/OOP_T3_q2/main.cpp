
#include <iostream>
#include <Windows.h>
using namespace std;

void zeroSmaller(int &a, int &b) {
    if (a < b) {
        a = 0;
    } else if (b < a) {
        b = 0;
    }
    // Якщо числа рівні, нічого не змінюється
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    int x, y;
    cout << "Введіть два цілих числа: ";
    cin >> x >> y;

    cout << "До виклику: x = " << x << ", y = " << y << endl;
    zeroSmaller(x, y);
    cout << "Після виклику: x = " << x << ", y = " << y << endl;

    return 0;
}