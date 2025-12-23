#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>

using namespace std;

/* =====================
   1. Будинок – Навчальний корпус
   ===================== */
class House {
protected:
    string address;
    int floors;

public:
    House(const string& a, int f) : address(a), floors(f) {}

    virtual void show() const {
        cout << "Тип: Будинок" << endl;
        cout << "Адреса: " << address << endl;
        cout << "Поверхів: " << floors << endl;
    }

    virtual ~House() {}
};

class AcademicBuilding : public House {
    int classrooms;

public:
    AcademicBuilding(const string& a, int f, int c)
        : House(a, f), classrooms(c) {}

    void show() const override {
        cout << "Тип: Навчальний корпус" << endl;
        cout << "Адреса: " << address << endl;
        cout << "Поверхів: " << floors << endl;
        cout << "Аудиторій: " << classrooms << endl;
    }
};

void task1_demo() {
    vector<House*> buildings;

    buildings.push_back(new House("Київ, Миропільська 5", 9));
    buildings.push_back(new AcademicBuilding("Київ, Львівська площа, 14", 5, 40));

    for (auto b : buildings) {
        b->show();
        cout << "------------------" << endl;
    }

    for (auto b : buildings)
        delete b;
}

/* =====================
   2. Цифровий пристрій – Мобільний телефон
   ===================== */
class DigitalDevice {
protected:
    string brand;

public:
    DigitalDevice(const string& b) : brand(b) {}

    virtual void show() const {
        cout << "Тип: Цифровий пристрій" << endl;
        cout << "Бренд: " << brand << endl;
    }

    virtual ~DigitalDevice() {}
};

class MobilePhone : public DigitalDevice {
    string model;

public:
    MobilePhone(const string& b, const string& m)
        : DigitalDevice(b), model(m) {}

    void show() const override {
        cout << "Тип: Мобільний телефон" << endl;
        cout << "Бренд: " << brand << endl;
        cout << "Модель: " << model << endl;
    }
};

void task2_demo() {
    vector<DigitalDevice*> devices;

    devices.push_back(new DigitalDevice("Asus"));
    devices.push_back(new MobilePhone("Xiaomi", "Poco_F5"));

    for (auto d : devices) {
        d->show();
        cout << "------------------" << endl;
    }

    for (auto d : devices)
        delete d;
}

/* =====================
   3. Шаблонна функція
   ===================== */
template <typename T>
void findInRange(const vector<T>& arr, T min, T max) {
    for (auto x : arr) {
        if (x >= min && x <= max)
            cout << x << " ";
    }
    cout << endl;
}

void task3_demo() {
    vector<int> a = {2, 5, 8, 12, 20};
    cout << "Елементи в діапазоні [5;15]: ";
    findInRange(a, 5, 15);

    vector<double> b = {1.1, 2.5, 3.9, 5.0};
    cout << "Елементи в діапазоні [2.0;4.0]: ";
    findInRange(b, 2.0, 4.0);
}

/* =====================
   4. Шаблон класу Pair
   ===================== */
template <typename T1, typename T2>
class Pair {
    T1 first;
    T2 second;

public:
    Pair(T1 a, T2 b) : first(a), second(b) {}

    void show() const {
        cout << "(" << first << ", " << second << ")" << endl;
    }
};

void task4_demo() {
    Pair<int, double> p1(10, 3.14);
    Pair<string, int> p2("Вік", 19);

    p1.show();
    p2.show();
}

int main() {
    task1_demo();
    task2_demo();
    task3_demo();
    task4_demo();

    return 0;
}
