#ifndef HOTEL_PERSON_H
#define HOTEL_PERSON_H
#pragma once
#include <string>
using namespace std;

// Абстрактний базовий клас для представлення спільних анкетних характеристик людини
class Person {
protected:
    string firstName; // Рядок для збереження імені людини
    string lastName; // Рядок для збереження прізвища людини
    string middleName; // Рядок для збереження по батькові людини
    string phone; // Рядок для збереження номера мобільного телефону

public:
    // Конструктор ініціалізує текстові поля персональних даних особи
    Person(const string& fn, const string& ln, const string& mn, const string& ph)
        : firstName(fn), lastName(ln), middleName(mn), phone(ph) {}

    // Віртуальний деструктор гарантує правильний запуск очищення пам'яті у нащадків
    virtual ~Person() {}

    // Метод формує та повертає повний текстовий рядок ПІБ людини
    string getFullName() const {
        return lastName + " " + firstName + " " + middleName;
    }

    // Метод для отримання збереженого контактного номера телефону
    string getPhone() const { return phone; }

    // Чистий віртуальний метод для обов'язкової реалізації друку даних у похідних класах
    virtual string getInfo() const = 0;
};
#endif //HOTEL_PERSON_H