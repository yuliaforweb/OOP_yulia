#include <iostream>
#include <Windows.h>
#include <list>
#include <algorithm>

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    int n;
    std::cout << "Введіть кількість елементів n: ";
    if (!(std::cin >> n) || n <= 0) {
        std::cout << "Помилка: n має бути натуральним числом." << std::endl;
        return 1;
    }

    std::list<double> numbers;
    std::cout << "Введіть " << n << " дійсних чисел:" << std::endl;
    for (int i = 0; i < n; ++i) {
        double val;
        std::cin >> val;
        numbers.push_back(val);
    }

    // 1. Перевірка умови: чи є хоча б один елемент < -3
    bool has_less_than_minus_three = std::any_of(numbers.begin(), numbers.end(), [](double x) {
        return x < -3.0;
    });

    // 2. Трансформація списку згідно з умовою
    if (has_less_than_minus_three) {
        // Якщо є елемент < -3: від'ємні замінюємо квадратами, інші не чіпаємо
        for (double &x : numbers) {
            if (x < 0) {
                x = x * x;
            }
        }
    } else {
        // В іншому випадку: множимо все на 0.1
        for (double &x : numbers) {
            x *= 0.1;
        }
    }

    // 3. Вивід результату у зворотному порядку
    std::cout << "Результат (у зворотному порядку):" << std::endl;
    for (auto it = numbers.rbegin(); it != numbers.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    return 0;
}