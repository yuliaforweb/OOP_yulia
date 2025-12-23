#include <iostream>
using namespace std;

class Calculator {
public:
    double acc;
    Calculator() {
        acc = 0;
    }
};

void exe(Calculator& c, char op, double value) {
    if (op == '+')
        c.acc = c.acc + value;
    else if (op == '-')
        c.acc = c.acc - value;
    else if (op == '*')
        c.acc = c.acc * value;
    else if (op == '/') {
        if (value != 0)
            c.acc = c.acc / value;
        else
            cout << "Помилка: ділення на нуль" << endl;
    }
}

int main() {
    Calculator c;

    exe(c, '+', 10);
    exe(c, '*', 2);
    exe(c, '-', 5);
    exe(c, '/', 5);

    cout << "Акумулятор = " << c.acc << endl;

    return 0;
}
