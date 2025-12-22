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

// Метод Сімпсона
double simpson(double a, double b, int n, int choice) {
    if (n % 2 != 0) n++; // n має бути парним
    double h = (b - a) / n;
    double sum = f(a, choice) + f(b, choice);
    for (int i = 1; i < n; i++) {
        if (i % 2 == 0) sum += 2 * f(a + i*h, choice);
        else sum += 4 * f(a + i*h, choice);
    }
    return sum * h / 3.0;
}

int main() {
    int funcChoice;
    double a, b;
    int n;

    cout << "Виберіть функцію для інтегрування:\n";
    cout << "1 - sin(x)\n2 - cos(x)\n3 - x^2\n";
    cout << "Ваш вибір: ";
    cin >> funcChoice;

    cout << "Вкажіть початок відрізка: ";
    cin >> a;
    cout << "Вкажіть кінець відрізка: ";
    cin >> b;
    cout << "Вкажіть кількість розбиттів (n, парне число): ";
    cin >> n;

    double result = simpson(a, b, n, funcChoice);
    cout << "Значення інтегралу: " << result << endl;

    return 0;
}
