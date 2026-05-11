#ifndef HOTEL_ROOM_H
#define HOTEL_ROOM_H
#pragma once
#include <string>
using namespace std;

enum class RoomStatus {
    FREE, BOOKED, OCCUPIED
};

class Room {
protected:
    int        roomNumber;
    int        floor;
    int        capacity;
    double     pricePerNight;
    RoomStatus status;
public:
    Room(int num, int fl, double price, int cap)
        : roomNumber(num), floor(fl),
          pricePerNight(price), capacity(cap),
          status(RoomStatus::FREE) {}
    virtual ~Room() {}

    int        getNumber()        const { return roomNumber; }
    int        getFloor()          const { return floor; }
    int        getCapacity()       const { return capacity; }
    double     getPricePerNight()  const { return pricePerNight; }
    RoomStatus getStatus()         const { return status; }
    void       setStatus(RoomStatus s)    { status = s; }

    virtual string getDescription() const = 0;
};
#endif //HOTEL_ROOM_H