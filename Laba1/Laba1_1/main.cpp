#include <iostream>
#include <cmath>
#include <limits> // для std::numeric_limits

using namespace std;

int main() {

    double x, y;

    cout << "=== Завдання 1, варіант 5 ===" << endl;
    cout << "Вкажіть значення x (x > 0): ";
    cin >> x;

    if (x <= 0) {

        cerr << "Помилка: x має бути більше 0" << endl;

        return 1;
    }
    // Чисельник
    double numerator = exp(-x) - 4*x - pow(log(x), 3);

    // Котангенс: cot(a) = 1 / tan(a)
    double cot_term = 1.0 / tan(x*x - 1);

    // Знаменник

    double denominator = log10(fabs(x + 1)) + cot_term;

    // Перевірка, що знаменник не дорівнює нулю

    if (fabs(denominator) < numeric_limits<double>::epsilon()) {

        cerr << "Помилка: знаменник дорівнює нулю." << endl;

        return 1;
    }

    y = numerator / denominator;
    cout << "Результат y = " << y << endl;
    return 0;
}