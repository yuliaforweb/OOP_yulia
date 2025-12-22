#include <iostream>
#include <iomanip>

using namespace std;

int main() {

    double num1, num2;
    char operation;

    cout << "Калькулятор" << endl;

    //Вквзвти перше число
    cout << "Вкажіть перше число: ";
    cin >> num1;

    // Вказати дію
    cout << "Вкажіть дію (+, -, *, /): ";
    cin >> operation;

    // Вказати друге число
    cout << "Вкажіть друге число: ";
    cin >> num2;

    double result = 0;
    bool valid = true;

    // Логіка обчислень
    if (operation == '+') {
        result = num1 + num2;
    } else if (operation == '-') {
        result = num1 - num2;
    } else if (operation == '*') {
        result = num1 * num2;
    } else if (operation == '/') {
        if (num2 != 0) {
            result = num1 / num2;
        } else {
            cout << "Помилка: на нуль ділити не можна!" << endl;
            valid = false;
        }
    } else {
        cout << "Помилка: невідома операція!" << endl;
        valid = false;
    }

    // Виведення результату
    if (valid) {
        cout << fixed << setprecision(2); // Округлення до 2 знаків після коми
        cout << "Результат: " << num1 << " " << operation << " " << num2 << " = " << result << endl;
    }

    return 0;
}