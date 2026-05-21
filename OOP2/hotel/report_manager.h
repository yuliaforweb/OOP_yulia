#ifndef HOTEL_REPORT_MANAGER_H
#define HOTEL_REPORT_MANAGER_H

#include "room.h"
#include "booking.h"
#include <vector>

class ReportManager {
private:
    const std::vector<Room*>& rooms;
    const std::vector<Booking*>& bookings;

public:
    ReportManager(const std::vector<Room*>& r, const std::vector<Booking*>& b);
    void generateOccupancyReport() const;
    void generateFinancialReport() const;
};

#endif