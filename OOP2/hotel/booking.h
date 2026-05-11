#ifndef HOTEL_BOOKING_H
#define HOTEL__BOOKING_H
#pragma once
#include "guest.h"
#include "room.h"

enum class BookingStatus {
    ACTIVE, CANCELLED, COMPLETED
};

class Booking {
private:
    int           bookingId;
    Guest*        guest;
    Room*         room;
    string        checkIn;
    string        checkOut;
    double        totalCost;
    BookingStatus status;

public:
    static int dateToInt(const string& d);
    
    Booking(int id, Guest* g, Room* r,
            const string& ci, const string& co);

    double calculateCost();
    bool   overlaps(const string& ci,
                    const string& co) const;

    int           getId()       const { return bookingId; }
    Guest*        getGuest()    const { return guest; }
    Room*         getRoom()     const { return room; }
    string        getCheckIn()  const { return checkIn; }
    string        getCheckOut() const { return checkOut; }
    double        getTotalCost()const { return totalCost; }
    BookingStatus getStatus()   const { return status; }

    void cancel()   { status = BookingStatus::CANCELLED; }
    void complete() { status = BookingStatus::COMPLETED; }
    void printInfo() const;
};
#endif //HOTEL__BOOKING_H