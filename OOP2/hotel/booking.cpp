#include "booking.h"
#include <ctime>
#include <iostream>
#include <iomanip>

Booking::Booking(int id, Guest* g, Room* r,
                 const string& ci, const string& co)
    : bookingId(id), guest(g), room(r),
      checkIn(ci), checkOut(co),
      totalCost(0.0),
      status(BookingStatus::ACTIVE) {}

int Booking::dateToInt(const string& d) {
    int day  = stoi(d.substr(0, 2));
    int mon  = stoi(d.substr(3, 2));
    int year = stoi(d.substr(6, 4));
    return year*10000 + mon*100 + day;
}

bool Booking::overlaps(const string& ci,
                       const string& co) const {
    int a1 = dateToInt(checkIn);
    int a2 = dateToInt(checkOut);
    int b1 = dateToInt(ci);
    int b2 = dateToInt(co);
    return (a1 < b2) && (b1 < a2);
}

double Booking::calculateCost() {
    int d1 = dateToInt(checkIn);
    int d2 = dateToInt(checkOut);
    struct tm t1 = {0}, t2 = {0};
    t1.tm_year = (d1 / 10000) - 1900;
    t1.tm_mon  = (d1 / 100 % 100) - 1;
    t1.tm_mday =  d1 % 100;
    t1.tm_isdst = -1;
    t2.tm_year = (d2 / 10000) - 1900;
    t2.tm_mon  = (d2 / 100 % 100) - 1;
    t2.tm_mday =  d2 % 100;
    t2.tm_isdst = -1;
    time_t tt1 = mktime(&t1);
    time_t tt2 = mktime(&t2);
    if (tt1 == -1 || tt2 == -1) return 0.0;
    double seconds = difftime(tt2, tt1);
    int days = static_cast<int>((seconds + 3600) / 86400);
    if (days < 0) days = 0;
    totalCost = days * room->getPricePerNight();
    return totalCost;
}

void Booking::printInfo() const {
    string st = (status==BookingStatus::ACTIVE)   ? "Активне"    :
                (status==BookingStatus::CANCELLED) ? "Скасоване" :
                                                     "Завершене";
    cout << "Бронювання #" << bookingId << endl;
    cout << "  Гість  : "
         << guest->getFullName() << endl;
    cout << "  Номер  : " << room->getNumber()
         << " (" << room->getDescription() << ")" << endl;
    cout << "  Заїзд  : " << checkIn  << endl;
    cout << "  Виїзд  : " << checkOut << endl;
    cout << fixed << setprecision(2);
    cout << "  Вартість: " << totalCost
         << " грн" << endl;
    cout << "  Статус  : " << st << endl;
}