#include <iostream>
#include <cmath>
using namespace std;

// Базовий клас
class Solid {
protected:
    string name;

public:
    Solid(string n) {
        name = n;
    }

    virtual void print() {
        cout << "Фігура обертання: " << name << endl;
    }

    virtual double volume() {
        return 0;
    }

    virtual ~Solid() {}
};

// Похідний клас Конус
class Cone : public Solid {
    double r, h;

public:
    Cone(double radius, double height)
        : Solid("Конус") {
        r = radius;
        h = height;
    }

    double volume() override {
        return (3.14 * r * r * h) / 3;
    }

    void print() override {
        cout << "Фігура обертання: " << name << endl;
        cout << "Радіус: " << r << endl;
        cout << "Висота: " << h << endl;
        cout << "Обʼєм: " << volume() << endl;
    }
};

int main() {
    // Масив указівників
    Solid* arr[2];

    arr[0] = new Solid("Невідома фігура");
    arr[1] = new Cone(3, 5);

    // Вивід у циклі
    for (int i = 0; i < 2; i++) {
        arr[i]->print();
        cout << "----------------" << endl;
    }

    // Очищення памʼяті
    for (int i = 0; i < 2; i++) {
        delete arr[i];
    }

    return 0;
}
