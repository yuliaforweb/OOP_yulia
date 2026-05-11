#ifndef HOTEL_GUEST_H
#define HOTEL_GUEST_H
#pragma once
#include "person.h"

class Guest : public Person {
private:
    int    guestId;
    string passportNumber;
    string dateOfBirth;
    string email;
public:
    Guest(int id,
          const string& fn, const string& ln,
          const string& mn, const string& ph,
          const string& pass,
          const string& dob,
          const string& em)
        : Person(fn, ln, mn, ph),
          guestId(id), passportNumber(pass),
          dateOfBirth(dob), email(em) {}

    int    getId()       const { return guestId; }
    string getPassport() const { return passportNumber; }
    string getEmail()    const { return email; }
    string getInfo()     const override;
};
#endif //HOTEL_GUEST_H