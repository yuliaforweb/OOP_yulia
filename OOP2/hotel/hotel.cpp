#include "hotel.h"
#include "auth_manager.h"
#include "logger.h"
#include <libpq-fe.h>
#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;
using json = nlohmann::json;

static bool isLeapYearInternal(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static int calculateDaysInternal(const string& checkIn, const string& checkOut) {
    int d1 = 0, m1 = 0, y1 = 0, d2 = 0, m2 = 0, y2 = 0;
    if (sscanf(checkIn.c_str(), "%d.%d.%d", &d1, &m1, &y1) == 3 &&
        sscanf(checkOut.c_str(), "%d.%d.%d", &d2, &m2, &y2) == 3) {

        const int daysInMonths[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        auto getAbsoluteDays = [&](int d, int m, int y) {
            int total = d;
            for (int i = 1; i < m; ++i) {
                if (i == 2 && isLeapYearInternal(y)) total += 29;
                else total += daysInMonths[i];
            }
            total += (y - 2026) * 365;
            for (int i = 2026; i < y; ++i) {
                if (isLeapYearInternal(i)) total += 1;
            }
            return total;
        };

        int days = getAbsoluteDays(d2, m2, y2) - getAbsoluteDays(d1, m1, y1);
        return days > 0 ? days : 1;
    }
    return 1;
}

HotelSystem::HotelSystem() {
}

void HotelSystem::loadData() {
}

string HotelSystem::getAvailableRoomsStr(const string& checkIn, const string& checkOut) {
    return "[{\"id\":1,\"desc\":\"Стандарт 2-місний\",\"available_count\":4,\"price\":800.0}]";
}

bool HotelSystem::addBookingFromStaff(const string& name, const string& phone, const string& passport, int roomNum, const string& checkIn, const string& checkOut) {
    const char* conninfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        Logger::error("Помилка з'єднання з базою даних готелю при спробі реєстрації");
        PQfinish(conn);
        return false;
    }

    string checkQuery = "SELECT id FROM guests WHERE passport = '" + passport + "';";
    PGresult* res = PQexec(conn, checkQuery.c_str());
    int guestId = -1;
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        guestId = atoi(PQgetvalue(res, 0, 0));
    }
    PQclear(res);

    if (guestId == -1) {
        string insertGuest = "INSERT INTO guests (fullname, phone, passport) VALUES ('" + name + "', '" + phone + "', '" + passport + "') RETURNING id;";
        res = PQexec(conn, insertGuest.c_str());
        if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
            guestId = atoi(PQgetvalue(res, 0, 0));
        }
        PQclear(res);
    }

    string priceQuery = "SELECT price FROM rooms WHERE number = " + to_string(roomNum) + ";";
    res = PQexec(conn, priceQuery.c_str());
    double roomPrice = 800.0;
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        roomPrice = atof(PQgetvalue(res, 0, 0));
    }
    PQclear(res);

    string insertBooking = "INSERT INTO bookings (guestid, roomnumber, checkin, checkout, totalcost, status, payment_method, payment_status) VALUES (" +
                           to_string(guestId) + ", " + to_string(roomNum) + ", '" + checkIn + "', '" + checkOut + "', " + to_string(roomPrice) + ", 'CONFIRMED', 'cash', 'paid');";
    res = PQexec(conn, insertBooking.c_str());
    bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);
    PQclear(res);
    PQfinish(conn);
    return success;
}

bool HotelSystem::addBookingFromWeb(const string& guest, const string& phone, const string& passport, int cat, const string& checkIn, const string& checkOut) {
    return addBookingFromWebExtended(guest, phone, passport, cat, checkIn, checkOut, "upon_check_in");
}

bool HotelSystem::addBookingFromWebExtended(const string& guest, const string& phone, const string& passport, int roomNum, const string& checkIn, const string& checkOut, const string& payMethod) {
    int targetRoom = roomNum;

    const char* conninfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        Logger::error("Помилка з'єднання з базою при вебовому бронюванні");
        PQfinish(conn);
        return false;
    }

    string checkQuery = "SELECT id FROM guests WHERE passport = '" + passport + "';";
    PGresult* res = PQexec(conn, checkQuery.c_str());
    int guestId = -1;
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        guestId = atoi(PQgetvalue(res, 0, 0));
    }
    PQclear(res);

    if (guestId == -1) {
        string insertGuest = "INSERT INTO guests (fullname, phone, passport) VALUES ('" + guest + "', '" + phone + "', '" + passport + "') RETURNING id;";
        res = PQexec(conn, insertGuest.c_str());
        if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
            guestId = atoi(PQgetvalue(res, 0, 0));
        }
        PQclear(res);
    }

    double pricePerNight = 800.0;
    if (targetRoom == 103) pricePerNight = 900.0;
    else if (targetRoom == 201 || targetRoom == 202) pricePerNight = 1500.0;
    else if (targetRoom == 301) pricePerNight = 3500.0;
    else if (targetRoom == 302) pricePerNight = 4200.0;

    int totalDays = calculateDaysInternal(checkIn, checkOut);
    double computedTotalCost = pricePerNight * totalDays;

    string dbMethod = "pending";
    string dbStatus = "upon_check_in";

    if (payMethod == "online") {
        dbMethod = "online";
        dbStatus = "paid";
    }

    string insertBooking = "INSERT INTO bookings (guestid, roomnumber, checkin, checkout, totalcost, status, payment_method, payment_status) VALUES (" +
                           to_string(guestId) + ", " + to_string(targetRoom) + ", '" + checkIn + "', '" + checkOut + "', " + to_string(computedTotalCost) + ", 'RESERVED', '" + dbMethod + "', '" + dbStatus + "');";
    res = PQexec(conn, insertBooking.c_str());
    bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);
    PQclear(res);
    PQfinish(conn);
    return success;
}

string HotelSystem::authenticateStaff(int roleIdx, const string& password) {
    AuthManager auth;
    return auth.checkCredentials(roleIdx, password);
}

bool HotelSystem::updateBookingData(int bookingId, const string& name, const string& phone, const string& passport, const string& checkIn, const string& checkOut) {
    const char* conninfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        Logger::error("Збій підключення до базы під час оновлення картки");
        PQfinish(conn);
        return false;
    }

    string updateBooking = "UPDATE bookings SET checkin = '" + checkIn + "', checkout = '" + checkOut + "' WHERE id = " + to_string(bookingId) + ";";
    PGresult* res = PQexec(conn, updateBooking.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQclear(res); PQfinish(conn);
        return false;
    }
    PQclear(res);

    string getGuestId = "SELECT guestid FROM bookings WHERE id = " + to_string(bookingId) + ";";
    res = PQexec(conn, getGuestId.c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        string gId = PQgetvalue(res, 0, 0);
        PQclear(res);
        string updateGuest = "UPDATE guests SET fullname = '" + name + "', phone = '" + phone + "', passport = '" + passport + "' WHERE id = " + gId + ";";
        res = PQexec(conn, updateGuest.c_str());
    }

    bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);
    PQclear(res);
    PQfinish(conn);
    return success;
}

bool HotelSystem::processReceptionPayment(int bookingId, const string& paymentMethod) {
    const char* conninfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        Logger::error("Збій підключення при проведенні оплати");
        PQfinish(conn);
        return false;
    }

    string sql = "UPDATE bookings SET payment_method = '" + paymentMethod + "', payment_status = 'paid' WHERE id = " + to_string(bookingId) + ";";
    PGresult* res = PQexec(conn, sql.c_str());
    bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);
    PQclear(res);
    PQfinish(conn);
    return success;
}

string HotelSystem::getGuestsStr() {
    const char* conninfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) return "[]";

    string query = "SELECT id, fullname, phone, passport FROM guests ORDER BY id;";
    PGresult* res = PQexec(conn, query.c_str());
    json jArr = json::array();

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; i++) {
            jArr.push_back({
                {"id", atoi(PQgetvalue(res, i, 0))},
                {"fullname", PQgetvalue(res, i, 1)},
                {"phone", PQgetvalue(res, i, 2)},
                {"passport", PQgetvalue(res, i, 3)}
            });
        }
    }
    PQclear(res); PQfinish(conn);
    return jArr.dump();
}

string HotelSystem::getReservationsStr(const string& passportFilter) {
    const char* conninfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) return "[]";

    string query = "SELECT b.id, g.fullname, g.phone, g.passport, b.roomnumber, b.checkin, b.checkout, b.totalcost, b.status, b.payment_method, b.payment_status "
                   "FROM bookings b JOIN guests g ON b.guestid = g.id";
    if (!passportFilter.empty()) {
        query += " WHERE g.passport = '" + passportFilter + "'";
    }
    query += " ORDER BY b.id DESC;";

    PGresult* res = PQexec(conn, query.c_str());
    json jArr = json::array();

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; i++) {
            jArr.push_back({
                {"id", atoi(PQgetvalue(res, i, 0))},
                {"fullname", PQgetvalue(res, i, 1)},
                {"phone", PQgetvalue(res, i, 2)},
                {"passport", PQgetvalue(res, i, 3)},
                {"room", atoi(PQgetvalue(res, i, 4))},
                {"checkin", PQgetvalue(res, i, 5)},
                {"checkout", PQgetvalue(res, i, 6)},
                {"total_pay", atof(PQgetvalue(res, i, 7))},
                {"status", PQgetvalue(res, i, 8)},
                {"payment_method", PQgetvalue(res, i, 9)},
                {"payment_status", PQgetvalue(res, i, 10)}
            });
        }
    }
    PQclear(res); PQfinish(conn);
    return jArr.dump();
}

json HotelSystem::getBookingJsonByPassport(const string& passport) {
    const char* conninfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);
    json jArr = json::array();

    if (PQstatus(conn) == CONNECTION_OK) {
        string sql = "SELECT b.id, b.roomnumber, r.description, b.checkin, b.checkout, b.status, b.payment_status, b.totalcost, g.fullname "
                     "FROM bookings b "
                     "JOIN guests g ON b.guestid = g.id "
                     "JOIN rooms r ON b.roomnumber = r.number "
                     "WHERE g.passport = '" + passport + "' ORDER BY b.id DESC;";

        PGresult* res = PQexec(conn, sql.c_str());
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            int rows = PQntuples(res);
            for (int i = 0; i < rows; i++) {
                string dbStatus = PQgetvalue(res, i, 5);
                string statusText = "Очікує підтвердження";
                if (dbStatus == "CONFIRMED") statusText = "Заселено / Активне";
                if (dbStatus == "CANCELLED") statusText = "Скасовано";

                jArr.push_back({
                    {"id", atoi(PQgetvalue(res, i, 0))},
                    {"room", atoi(PQgetvalue(res, i, 1))},
                    {"room_desc", PQgetvalue(res, i, 2)},
                    {"period", string(PQgetvalue(res, i, 3)) + " - " + PQgetvalue(res, i, 4)},
                    {"status_text", statusText},
                    {"payment_status", PQgetvalue(res, i, 6)},
                    {"total_pay", atof(PQgetvalue(res, i, 7))},
                    {"guest_name", PQgetvalue(res, i, 8)}
                });
            }
        }
        PQclear(res);
    }
    PQfinish(conn);
    return jArr;
}

string HotelSystem::getOccupancyReportStr(const string& start, const string& end) {
    const char* conninfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) return "[]";

    string query = "SELECT r.number, r.description, "
                   "CASE WHEN COUNT(b.id) > 0 THEN 'Зайнятий' ELSE 'Вільний' END "
                   "FROM rooms r LEFT JOIN bookings b ON r.number = b.roomnumber "
                   "AND b.status IN ('RESERVED', 'CONFIRMED') "
                   "GROUP BY r.number, r.description ORDER BY r.number;";

    PGresult* res = PQexec(conn, query.c_str());
    json jArr = json::array();

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; i++) {
            jArr.push_back({
                {"room", atoi(PQgetvalue(res, i, 0))},
                {"desc", PQgetvalue(res, i, 1)},
                {"state", PQgetvalue(res, i, 2)}
            });
        }
    }
    PQclear(res); PQfinish(conn);
    return jArr.dump();
}

string HotelSystem::getFinancialReportStr(const string& start, const string& end) {
    const char* conninfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) return "{}";

    string sql = "SELECT b.id, b.roomnumber, g.fullname, b.totalcost, "
                 "CASE WHEN TRIM(b.payment_method) = 'cash' THEN 'Готівка' ELSE 'Онлайн' END, "
                 "b.checkin "
                 "FROM bookings b JOIN guests g ON b.guestid = g.id "
                 "WHERE b.payment_status = 'paid' AND b.status != 'CANCELLED' AND "
                 "TO_DATE(b.checkin, 'DD.MM.YYYY') >= TO_DATE('" + start + "', 'DD.MM.YYYY') AND "
                 "TO_DATE(b.checkin, 'DD.MM.YYYY') <= TO_DATE('" + end + "', 'DD.MM.YYYY') ORDER BY b.id;";

    PGresult* res = PQexec(conn, sql.c_str());
    json responseObj = json::object();
    json rowsArray = json::array();
    double totalIncome = 0.0;

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; i++) {
            double cost = atof(PQgetvalue(res, i, 3));
            totalIncome += cost;

            rowsArray.push_back({
                {"id", atoi(PQgetvalue(res, i, 0))},
                {"room", atoi(PQgetvalue(res, i, 1))},
                {"guest", PQgetvalue(res, i, 2)},
                {"amount", cost},
                {"method", PQgetvalue(res, i, 4)},
                {"payment_date", PQgetvalue(res, i, 5)}
            });
        }
    }
    PQclear(res); PQfinish(conn);

    responseObj["report_period_start"] = start;
    responseObj["report_period_end"] = end;
    responseObj["total_income"] = totalIncome;
    responseObj["list"] = rowsArray;

    return responseObj.dump();
}

bool HotelSystem::exportPaymentsToExcel(const string& filename) {
    const char* conninfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        Logger::error("Збій підключення при генерації звіту");
        PQfinish(conn);
        return false;
    }

    string query = "SELECT b.roomnumber, g.fullname, b.totalcost, CASE WHEN TRIM(b.payment_method) = 'cash' THEN 'Готівка' ELSE 'Онлайн' END, b.checkin FROM bookings b JOIN guests g ON b.guestid = g.id WHERE b.payment_status = 'paid' AND b.status != 'CANCELLED' ORDER BY b.id;";

    PGresult* res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res); PQfinish(conn);
        return false;
    }

    string htmlFilename = "payments_report.html";
    ofstream html(htmlFilename);
    if (!html.is_open()) {
        PQclear(res); PQfinish(conn);
        return false;
    }

    html << "<html><head><meta charset='utf-8'><style>";
    html << "body { font-family: sans-serif; padding: 30px; background-color: #f4f6f9; color: #333; }";
    html << "h2 { color: #1a73e8; }";
    html << "table { width: 100%; border-collapse: collapse; margin-top: 20px; background: white; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }";
    html << "th, td { border: 1px solid #ddd; padding: 12px; text-align: left; }";
    html << "th { background-color: #1a73e8; color: white; }";
    html << "tr:nth-child(even) { background-color: #f9f9f9; }</style></head><body>";
    html << "<h2>Фінансовий звіт інформаційної системи комплексу \"Готель\"</h2>";
    html << "<h3>Реєстр проведених та закритих оплат персоналом:</h3>";
    html << "<table><tr><th>Номер</th><th>ПІБ гостя</th><th>Сума оплати</th><th>Спосіб оплати</th><th>Дата оплати</th></tr>";

    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        html << "<tr><td>" << PQgetvalue(res, i, 0) << "</td><td>"
             << PQgetvalue(res, i, 1) << "</td><td>"
             << PQgetvalue(res, i, 2) << " грн</td><td>"
             << PQgetvalue(res, i, 3) << "</td><td>"
             << PQgetvalue(res, i, 4) << "</td></tr>";
    }

    html << "</table><script>window.print();</script></body></html>";
    html.close();
    PQclear(res); PQfinish(conn);

    Logger::info("Аналітичний веб-звіт успішно сформовано: " + htmlFilename);

#if defined(_WIN32) || defined(_WIN64)
    string openCmd = "start " + htmlFilename;
    system(openCmd.c_str());
#elif defined(__linux__)
    string openCmd = "xdg-open " + htmlFilename;
    system(openCmd.c_str());
#endif

    return true;
}