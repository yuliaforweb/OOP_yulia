#include <iostream>

using namespace std;

class MyClass {
private:
    double a;
    double b;

public:
    void set(double x, double y) {
        a = x;
        b = y;
    }

    void print() const {
        cout << "a = " << a << ", b = " << b << endl;
    }

    double sum() const {
        return a + b;
    }

    double maxValue() const {
        return (a > b) ? a : b;
    }
};

int main() {
    MyClass obj;
    double x, y;

    cout << "Введіть перше число: ";
    cin >> x;
    cout << "Введіть друге число: ";
    cin >> y;

    obj.set(x, y);

    obj.print();
    cout << "Сума: " << obj.sum() << endl;
    cout << "Найбільше значення: " << obj.maxValue() << endl;

    return 0;
}
