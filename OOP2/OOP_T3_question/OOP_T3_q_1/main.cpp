#include <iostream>
#include <Windows.h>
using namespace std;

// Функція з параметром за умовчанням
double power(double n, int p = 2) {
    double result = 1.0;
    for (int i = 0; i < p; i++) {
        result *= n;
    }
    return result;
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    double num;
    int p;
    char choice;

    cout << "Введіть число: ";
    cin >> num;
    cout << "Бажаєте ввести степінь? (y/n): ";
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        cout << "Введіть степінь: ";
        cin >> p;
        cout << "Результат: " << power(num, p) << endl;
    } else {
        // Виклик з використанням значення за умовчанням
        cout << "Результат (квадрат): " << power(num) << endl;
    }

    return 0;
}