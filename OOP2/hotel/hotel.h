#ifndef HOTEL_H
#define HOTEL_H

#include <string>
#include "external/json.hpp"

class HotelSystem {
public:
    HotelSystem();
    void loadData();
    std::string getAvailableRoomsStr(const std::string& checkIn, const std::string& checkOut);
    bool addBookingFromStaff(const std::string& name, const std::string& phone, const std::string& passport, int roomNum, const std::string& checkIn, const std::string& checkOut);

    bool addBookingFromWeb(const std::string& guest, const std::string& phone, const std::string& passport, int cat, const std::string& checkIn, const std::string& checkOut);
    bool addBookingFromWebExtended(const std::string& guest, const std::string& phone, const std::string& passport, int cat, const std::string& checkIn, const std::string& checkOut, const std::string& payMethod);

    bool updateBookingData(int bookingId, const std::string& name, const std::string& phone, const std::string& passport, const std::string& checkIn, const std::string& checkOut);
    bool exportPaymentsToExcel(const std::string& filename);
    std::string getGuestsStr();
    std::string getReservationsStr(const std::string& passportFilter = "");
    std::string getOccupancyReportStr(const std::string& start, const std::string& end);
    std::string getFinancialReportStr(const std::string& start, const std::string& end);
    nlohmann::json getBookingJsonByPassport(const std::string& passport);

    bool processReceptionPayment(int bookingId, const std::string& method);
    std::string authenticateStaff(int roleIdx, const std::string& password);
};

#endif // HOTEL_H