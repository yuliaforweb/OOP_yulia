#include <iostream>

using namespace std;

int main() {
    int num;

    cout << "Вкажіть двоцифрове число: ";
    cin >> num;

    if (num < 10 || num > 99) {
        cout << "Помилка: введене число не є двоцифровим." << endl;
        return 0;
    }

    int tens = num / 10;       // кількість десятків
    int units = num % 10;      // кількість одиниць
    int sum = tens + units;    // сума цифр
    int product = tens * units; // добуток цифр

    cout << "Десятки: " << tens << endl;
    cout << "Одиниці: " << units << endl;
    cout << "Сума цифр: " << sum << endl;
    cout << "Добуток цифр: " << product << endl;

    return 0;
}
