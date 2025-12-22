#include <iostream>
#include <cmath>
using namespace std;

// Функції
double f(double x, int choice) {
    if (choice == 1) return sin(x);
    if (choice == 2) return cos(x);
    if (choice == 3) return x*x;
    return 0;
}

int main() {
    int funcChoice;
    double a, b, step;

    cout << "Виберіть функцію для табулювання:\n";
    cout << "1 - sin(x)\n2 - cos(x)\n3 - x^2\n";
    cout << "Ваш вибір: ";
    cin >> funcChoice;

    cout << "Вкажіть, будь ласка, початок відрізка: ";
    cin >> a;
    cout << "Вкажіть, будь ласка, кінець відрізка: ";
    cin >> b;
    cout << "Вкажіть, будь ласка, крок табулювання: ";
    cin >> step;

    cout << "\nX\tY\n";
    for (double x = a; x <= b; x += step) {
        cout << x << "\t" << f(x, funcChoice) << endl;
    }

    return 0;
}
