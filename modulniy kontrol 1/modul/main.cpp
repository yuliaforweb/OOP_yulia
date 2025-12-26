#include <iostream>
#include <string>
using namespace std;

// Базовий клас
class University {
protected:
    string name;

public:
    University(string n) {
        name = n;
    }

    virtual void print() {
        cout << "Університет: " << name << endl;
    }

    virtual ~University() {}
};

// Клас Факультет
class Faculty : public University {
protected:
    string facultyName;

public:
    Faculty(string univ, string fac) : University(univ) {
        facultyName = fac;
    }

    void print() override {
        cout << "Університет: " << name << endl;
        cout << "Факультет: " << facultyName << endl;
    }
};

// Клас Кафедра
class Department : public Faculty {
    string departmentName;

public:
    Department(string univ, string fac, string dep)
        : Faculty(univ, fac) {
        departmentName = dep;
    }

    void print() override {
        cout << "Університет: " << name << endl;
        cout << "Факультет: " << facultyName << endl;
        cout << "Кафедра: " << departmentName << endl;
    }
};

int main() {
    // Масив указівників
    University* arr[3];

    arr[0] = new University("МНТУ ім. Юрія Бугая");
    arr[1] = new Faculty("МНТУ ім. Юрія Бугая", "Факультет інформаційних технологій");
    arr[2] = new Department("МНТУ ім. Юрія Бугая", "Факультет інформаційних технологіій",
        "Кафедра Інженерії програмного забезпечення");


    // Вивід у циклі
    for (int i = 0; i < 3; i++) {
        arr[i]->print();
        cout << "----------------" << endl;
    }

    // Звільнення памʼяті
    for (int i = 0; i < 3; i++) {
        delete arr[i];
    }

    return 0;
}
