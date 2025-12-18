#include <iostream>
#include <cmath>
#include <iomanip> // Для fixed та setprecision

using namespace std;

int main()
{
    // Константи
    const double a = 1.75;
    const double b = 0.4;
    const double e_const = exp(1.0);

    double x, y, z;

    cout << "=== Завдання 1, варіант 5 ===" << endl;

    // Обчислюємо y
    y = 2 * a * pow(a + b, 1.0 / 3.0);

    // Обчислюємо x
    double arg_x = (exp(a) + exp(b)) / sqrt(a + e_const);
    x = atan(1.0 / arg_x);

    // Перевірка для x
    if (x <= 0) {
        cout << "Помилка обчислення Z: x <= 0" << endl;
    } else
        {

        // Обчислення z
        double numerator_z = fabs(x - 1) + exp(-y);
        double denominator_z = 12.34 - log10(sqrt(x));

        //перевірка знаменника
        if (fabs(denominator_z) < 1e-9) {
            cout << "Помилка: ділення на нуль (або дуже маленьке число)." << endl;
        } else {
            z = numerator_z / denominator_z;

            // Вивід результатів з форматуванням до 2 знаків
            cout << fixed << setprecision(2);
            cout << "Вхідні дані:" << endl;
            cout << "a = " << a << ", b = " << b << endl;
            cout << "--------------------" << endl;
            cout << "Результати:" << endl;
            cout << "y = " << y << endl;
            cout << "x = " << x << endl;
            cout << "z = " << z << endl;
        }
    }

    return 0;
}
