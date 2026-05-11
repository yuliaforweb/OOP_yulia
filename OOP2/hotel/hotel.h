#ifndef HOTEL_HOTEL_H
#define HOTEL_HOTEL_H
#pragma once
#include <vector>
#include <string>
#include "rooms.h"
#include "booking.h"

class HotelSystem {
private:
    vector<Room*>    rooms;
    vector<Guest*>   guests;
    vector<Booking*> bookings;
    int nextGuestId   = 1;
    int nextBookingId = 1;

    Guest* findOrCreateGuest();
    bool   isValidDate(const string& d);
    void   split(const string& s, char del,
                 vector<string>& out);
public:
    HotelSystem()  {}
    ~HotelSystem();

    Room* operator[](int index);

    void addRoom(Room* r);

    vector<Room*> checkAvailability(
        const string& ci, const string& co);

    void showAvailableRooms();
    void createBooking();
    void cancelBooking();
    void checkIn();
    void checkOut();
    void showGuests()    const;
    void showBookings()  const;
    void generateReport() const;

    void saveData()  const;
    void loadData();
};
#endif //HOTEL_HOTEL_H