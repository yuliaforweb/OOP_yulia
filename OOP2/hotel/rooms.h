#ifndef HOTEL_ROOMS_H
#define HOTEL_ROOMS_H
#pragma once
#include "room.h"

// Стандартний номер
class StandardRoom : public Room {
public:
    StandardRoom(int n, int fl, double pr, int cap)
        : Room(n, fl, pr, cap) {}

    string getDescription() const override {
        return "Стандартний, " +
               to_string(capacity) + " місць";
    }
};

// Номер категорії «Делюкс»
class DeluxeRoom : public Room {
    bool hasBalcony;
public:
    DeluxeRoom(int n, int fl, double pr,
               int cap, bool bal)
        : Room(n, fl, pr, cap),
          hasBalcony(bal) {}

    bool getHasBalcony() const { return hasBalcony; }
    string getDescription() const override;
};

// Апартаменти «Люкс»
class Suite : public Room {
    bool hasJacuzzi;
    int  roomCount;
public:
    Suite(int n, int fl, double pr,
          int cap, bool jac, int rc)
        : Room(n, fl, pr, cap),
          hasJacuzzi(jac), roomCount(rc) {}

    bool getHasJacuzzi() const { return hasJacuzzi; }
    int  getRoomCount()  const { return roomCount; }
    string getDescription() const override;
};
#endif //HOTEL_ROOMS_H