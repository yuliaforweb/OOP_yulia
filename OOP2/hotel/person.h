#ifndef HOTEL_PERSON_H
#define HOTEL_PERSON_H
#pragma once
#include <string>
using namespace std;

class Person {
protected:
    string firstName, lastName, middleName, phone;
public:
    Person(const string& fn, const string& ln,
           const string& mn, const string& ph)
        : firstName(fn), lastName(ln),
          middleName(mn), phone(ph) {}
    virtual ~Person() {}
    string getFullName() const {
        return lastName+" "+firstName+" "+middleName;
    }
    string getPhone() const { return phone; }
    virtual string getInfo() const = 0;
};
#endif //HOTEL_PERSON_H