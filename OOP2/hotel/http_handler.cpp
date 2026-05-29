#include "http_handler.h"
#include "web_page.h"
#include "external/json.hpp"
#include "hotel.h"
#include "logger.h"
#include <libpq-fe.h>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <vector>

#if !defined(_WIN32) && !defined(_WIN64)
    #include <unistd.h>
    #define closesocket close
#endif

using namespace std;
using json = nlohmann::json;

string sanitizeHtml(const string& str) {
    string sanitized;
    sanitized.reserve(str.length());
    for (char c : str) {
        switch (c) {
            case '&':  sanitized.append("&amp;");  break;
            case '<':  sanitized.append("&lt;");   break;
            case '>':  sanitized.append("&gt;");   break;
            default:   sanitized.push_back(c);     break;
        }
    }
    return sanitized;
}

void handleHttpGuest(SOCKET clientSocket, HotelSystem& hotel, const std::string& rawRequest) {
    size_t pos = rawRequest.find("\r\n\r\n");
    string body = (pos != string::npos) ? rawRequest.substr(pos + 4) : "";

    if (rawRequest.find("POST /api") != string::npos) {
        json replyJson;
        try {
            json requestJson = json::parse(body);
            string cmd = requestJson.value("command", "");

            Logger::info("Отримано команду від оператора системи: " + cmd);

            if (cmd == "LOGIN") {
                int roleIdx = requestJson.value("role", 1);
                string pass = requestJson.value("password", "");

                string roleText = hotel.authenticateStaff(roleIdx, pass);

                if (!roleText.empty()) {
                    replyJson = { {"status", "OK"}, {"role", roleText}, {"message", "Авторизацію успішно пройдено"} };
                    Logger::info("Успішний вхід користувача. Роль визначено як: " + roleText);
                } else {
                    replyJson = { {"status", "ERROR"}, {"message", "Невірний пароль чи таємний код доступу!"} };
                    Logger::error("Відмова в доступі: неправильні облікові дані для посади ID " + to_string(roleIdx));
                }
            }
            else if (cmd == "GUEST_CHECK_STATUS") {
                string guestPassport = requestJson.value("passport", "");
                Logger::info("Отримано веб-запит перевірки статусу броні для паспорта: " + guestPassport);
                json bData = hotel.getBookingJsonByPassport(guestPassport);
                replyJson = { {"status", "OK"}, {"bookings", bData}, {"arr", bData} };
            }
            else if (cmd == "CHANGE_PASSWORD") {
                int targetRole = requestJson.value("role", 1);
                int currentRole = requestJson.value("current_role", 1);
                string oldPass = requestJson.value("old_password", "");
                string newPass = requestJson.value("new_password", "");

                const char* connInfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
                PGconn* conn = PQconnectdb(connInfo);
                if (PQstatus(conn) != CONNECTION_OK) {
                    Logger::error("Збій підключення до бази при зміні пароля");
                    replyJson = { {"status", "ERROR"}, {"message", "Збій підключення до бази"} };
                    PQfinish(conn);
                } else {
                    bool canChange = false;
                    if (currentRole == targetRole) {
                        string sqlCheck = "SELECT passwordhash FROM staff_users WHERE roleid = " + to_string(currentRole) + ";";
                        PGresult* resCheck = PQexec(conn, sqlCheck.c_str());
                        if (PQresultStatus(resCheck) == PGRES_TUPLES_OK && PQntuples(resCheck) > 0) {
                            string currentDbHash = PQgetvalue(resCheck, 0, 0);
                            if (currentDbHash == oldPass) canChange = true;
                        }
                        PQclear(resCheck);
                    } else if (currentRole == 1) {
                        canChange = true;
                    }

                    if (!canChange) {
                        replyJson = { {"status", "ERROR"}, {"message", "Помилка безпеки: Поточний пароль невірний!"} };
                        Logger::error("Помилка безпеки: спроба несанкціонованої зміни пароля для ролі ID " + to_string(targetRole));
                    } else {
                        string sql = "UPDATE staff_users SET passwordhash = '" + newPass + "' WHERE roleid = " + to_string(targetRole) + ";";
                        PGresult* res = PQexec(conn, sql.c_str());
                        if (PQresultStatus(res) == PGRES_COMMAND_OK) {
                            replyJson = { {"status", "OK"}, {"message", "Пароль системи успішно оновлено"} };
                            Logger::info("Пароль для ролі ID " + to_string(targetRole) + " успішно змінено");
                        } else {
                            replyJson = { {"status", "ERROR"}, {"message", "Збій виконання SQL-запиту"} };
                            Logger::error("Збій виконання SQL UPDATE при зміні пароля");
                        }
                        PQclear(res);
                    }
                    PQfinish(conn);
                }
            }
            else if (cmd == "CHECK_FREE_ROOMS") {
                string inDate = requestJson.value("check_in", "");
                string outDate = requestJson.value("check_out", "");

                const char* connInfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
                PGconn* conn = PQconnectdb(connInfo);
                int booked101 = 0, booked102 = 0, booked103 = 0, booked201 = 0, booked202 = 0, booked301 = 0, booked302 = 0;

                if (PQstatus(conn) == CONNECTION_OK) {
                    string sql = "SELECT roomnumber FROM bookings WHERE status IN ('RESERVED', 'CONFIRMED') AND "
                                 "TO_DATE(checkout, 'DD.MM.YYYY') > TO_DATE('" + inDate + "', 'DD.MM.YYYY') AND "
                                 "TO_DATE(checkin, 'DD.MM.YYYY') < TO_DATE('" + outDate + "', 'DD.MM.YYYY');";
                    PGresult* res = PQexec(conn, sql.c_str());
                    int num = PQntuples(res);
                    for (int i = 0; i < num; i++) {
                        int r = atoi(PQgetvalue(res, i, 0));
                        if (r == 101) booked101 = 1;
                        if (r == 102) booked102 = 1;
                        if (r == 103) booked103 = 1;
                        if (r == 201) booked201 = 1;
                        if (r == 202) booked202 = 1;
                        if (r == 301) booked301 = 1;
                        if (r == 302) booked302 = 1;
                    }
                    PQclear(res);
                    PQfinish(conn);
                }

                int countCat1 = (1 - booked101) + (1 - booked102);

                json roomsArray = json::array();
                roomsArray.push_back({{"id", 1}, {"desc", "Стандартний 2-місний"}, {"available_count", countCat1}, {"price", 800.0}});
                roomsArray.push_back({{"id", 2}, {"desc", "Стандартний 3-місний"}, {"available_count", 1 - booked103}, {"price", 900.0}});
                roomsArray.push_back({{"id", 3}, {"desc", "Делюкс 2-місний з балконом"}, {"available_count", 1 - booked201}, {"price", 1500.0}});
                roomsArray.push_back({{"id", 4}, {"desc", "Делюкс 3-місний без балкона"}, {"available_count", 1 - booked202}, {"price", 1500.0}});
                roomsArray.push_back({{"id", 5}, {"desc", "Люкс 4-місний (джакузі, 2 номери)"}, {"available_count", 1 - booked301}, {"price", 3500.0}});
                roomsArray.push_back({{"id", 6}, {"desc", "Люкс 6-місний (джакузі, 3 номери)"}, {"available_count", 1 - booked302}, {"price", 4200.0}});

                replyJson = { {"status", "OK"}, {"rooms", roomsArray} };
            }
            else if (cmd == "CREATE_BOOKING") {
                string guestName = requestJson.value("guest_name", "");
                string phone = requestJson.value("phone", "");
                string passport = requestJson.value("passport", "");
                int roomNum = requestJson.value("room_number", 101);
                string ci = requestJson.value("check_in", "");
                string co = requestJson.value("check_out", "");
                string pChoice = requestJson.value("payment_method_choice", "upon_check_in");

                const char* connInfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
                PGconn* conn = PQconnectdb(connInfo);

                vector<int> targetRooms;
                if (roomNum == 101 || roomNum == 102) {
                    targetRooms = {101, 102};
                } else {
                    targetRooms = {roomNum};
                }

                int finalRoomToBook = -1;

                if (conn && PQstatus(conn) == CONNECTION_OK) {
                    for (int room : targetRooms) {
                        string checkSql = "SELECT 1 FROM bookings WHERE roomnumber = " + to_string(room) +
                                          " AND status IN ('RESERVED', 'CONFIRMED') AND "
                                          "TO_DATE(checkout, 'DD.MM.YYYY') > TO_DATE('" + ci + "', 'DD.MM.YYYY') AND "
                                          "TO_DATE(checkin, 'DD.MM.YYYY') < TO_DATE('" + co + "', 'DD.MM.YYYY');";
                        PGresult* checkRes = PQexec(conn, checkSql.c_str());
                        if (PQresultStatus(checkRes) == PGRES_TUPLES_OK && PQntuples(checkRes) == 0) {
                            finalRoomToBook = room;
                            PQclear(checkRes);
                            break;
                        }
                        PQclear(checkRes);
                    }
                    PQfinish(conn);
                }

                if (finalRoomToBook == -1) {
                    replyJson = { {"status", "ERROR"}, {"message", "Помилка: Усі номери цієї категорії на вказані дати вже зайняті!"} };
                    Logger::error("Відмова у бронюванні: немає вільних номерів у обраній категорії");
                } else {
                    bool ok = hotel.addBookingFromWebExtended(guestName, phone, passport, finalRoomToBook, ci, co, pChoice);
                    if (ok) {
                        replyJson = { {"status", "OK"}, {"message", "Картку проживання успішно внесено за номером " + to_string(finalRoomToBook)} };
                        Logger::info("Картку успішно внесено за номером " + to_string(finalRoomToBook));
                    } else {
                        replyJson = { {"status", "ERROR"}, {"message", "Помилка: Не вдалося зберегти бронь!"} };
                        Logger::error("Збій збереження картки проживання в базі даних");
                    }
                }
            }
            else if (cmd == "WEB_BOOKING") {
                string guest = requestJson.value("guest", "");
                string phone = requestJson.value("phone", "");
                string pass = requestJson.value("passport", "");
                int cat = requestJson.value("category", 1);
                string inDate = requestJson.value("checkin", "");
                string outDate = requestJson.value("checkout", "");
                string payMethod = requestJson.value("payment_method", "upon_check_in");

                Logger::info("Отримано веб-запит на бронювання від клієнта: " + guest);

                bool isValidWebData = true;
                if (guest.empty() || phone.empty() || pass.empty() || inDate.length() != 10 || outDate.length() != 10) {
                    isValidWebData = false;
                }
                if (inDate.length() == 10 && (inDate[2] != '.' || inDate[5] != '.')) isValidWebData = false;
                if (outDate.length() == 10 && (outDate[2] != '.' || outDate[5] != '.')) isValidWebData = false;

                if (!isValidWebData) {
                    replyJson = { {"status", "ERROR"}, {"message", "Помилка: Некоректний формат даних або порушено маску введення на сайті!"} };
                    Logger::error("Блокування веб-запиту: порушення масок введення");
                } else {
                    bool ok = hotel.addBookingFromWebExtended(guest, phone, pass, cat, inDate, outDate, payMethod);
                    replyJson = { {"status", ok ? "OK" : "ERROR"}, {"message", ok ? "Бронювання успішно створено" : "Немає вільних номерів на ці дати"} };
                }
            }
            else if (cmd == "SHOW_RESERVATIONS") {
                string filter = requestJson.value("passport", "");
                if (filter.empty()) filter = requestJson.value("passport_filter", "");
                Logger::info("Запит журналу замовлень. Фільтр паспорта: " + (filter.empty() ? "всі" : filter));
                replyJson = { {"status", "OK"}, {"arr", json::parse(hotel.getReservationsStr(filter))} };
            }
            else if (cmd == "SHOW_GUESTS") {
                replyJson = { {"status", "OK"}, {"arr", json::parse(hotel.getGuestsStr())} };
            }
            else if (cmd == "REPORT_OCCUPANCY") {
                string s = requestJson.value("start", "01.01.2026");
                string e = requestJson.value("end", "31.12.2026");
                replyJson = { {"status", "OK"}, {"arr", json::parse(hotel.getOccupancyReportStr(s, e))} };
            }
            else if (cmd == "REPORT_FINANCIAL") {
                string s = requestJson.value("start", "01.01.2026");
                string e = requestJson.value("end", "31.12.2026");
                replyJson = { {"status", "OK"}, {"data", json::parse(hotel.getFinancialReportStr(s, e))} };
            }
            else if (cmd == "CONFIRM_BOOKING") {
                int bId = requestJson.value("id", 0);
                const char* connInfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
                PGconn* conn = PQconnectdb(connInfo);
                bool ok = false;
                if (conn && PQstatus(conn) == CONNECTION_OK) {
                    string sql = "UPDATE bookings SET status = 'CONFIRMED' WHERE id = " + to_string(bId) + ";";
                    PGresult* res = PQexec(conn, sql.c_str());
                    if (PQresultStatus(res) == PGRES_COMMAND_OK) ok = true;
                    PQclear(res);
                    PQfinish(conn);
                }
                replyJson = { {"status", ok ? "OK" : "ERROR"}, {"message", ok ? "Резерв підтверджено" : "Збій бази даних"} };
            }
            else if (cmd == "CANCEL_BOOKING") {
                int bId = requestJson.value("id", 0);
                const char* connInfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
                PGconn* conn = PQconnectdb(connInfo);
                bool ok = false;
                if (conn && PQstatus(conn) == CONNECTION_OK) {
                    string sql = "UPDATE bookings SET status = 'CANCELLED' WHERE id = " + to_string(bId) + ";";
                    PGresult* res = PQexec(conn, sql.c_str());
                    if (PQresultStatus(res) == PGRES_COMMAND_OK) ok = true;
                    PQclear(res);
                    PQfinish(conn);
                }
                replyJson = { {"status", ok ? "OK" : "ERROR"}, {"message", ok ? "Бронювання скасовано" : "Збій бази даних"} };
            }
            else if (cmd == "PROCESS_PAYMENT") {
                int bId = requestJson.value("id", 0);
                string method = requestJson.value("method", "cash");

                const char* connInfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
                PGconn* conn = PQconnectdb(connInfo);
                bool isCancelled = false;

                if (conn && PQstatus(conn) == CONNECTION_OK) {
                    string checkSql = "SELECT status FROM bookings WHERE id = " + to_string(bId) + ";";
                    PGresult* checkRes = PQexec(conn, checkSql.c_str());
                    if (PQresultStatus(checkRes) == PGRES_TUPLES_OK && PQntuples(checkRes) > 0) {
                        string currentStatus = PQgetvalue(checkRes, 0, 0);
                        if (currentStatus == "CANCELLED") isCancelled = true;
                    }
                    PQclear(checkRes);
                    PQfinish(conn);
                }

                if (isCancelled) {
                    replyJson = { {"status", "ERROR"}, {"message", "Помилка: Неможливо провести оплату для скасованого замовлення!"} };
                    Logger::error("Блокування транзакції: спроба оплати скасованої броні ID " + to_string(bId));
                } else {
                    bool ok = hotel.processReceptionPayment(bId, method);
                    replyJson = { {"status", ok ? "OK" : "ERROR"}, {"message", ok ? "Оплату проведено" : "Помилка транзакції"} };
                    if (ok) Logger::info("Касовий ордер успішно оплачено через термінал/готівку");
                }
            }
            else if (cmd == "UPDATE_BOOKING_DATA") {
                int bId = requestJson.value("id", 0);
                string name = requestJson.value("fullname", "");
                string phone = requestJson.value("phone", "");
                string pass = requestJson.value("passport", "");
                string inDate = requestJson.value("checkin", "");
                string outDate = requestJson.value("checkout", "");
                bool ok = hotel.updateBookingData(bId, name, phone, pass, inDate, outDate);
                replyJson = { {"status", ok ? "OK" : "ERROR"}, {"message", ok ? "Картку оновлено" : "Збій бази даних"} };
            }
            else if (cmd == "EXPORT_PAYMENTS_XLS") {
                bool ok = hotel.exportPaymentsToExcel("payments_report.xls");
                replyJson = { {"status", ok ? "OK" : "ERROR"}, {"message", ok ? "Звіт збережено" : "Збій експорту"} };
            }
        } catch (const exception& err) {
            Logger::error("Критична помилка обробки JSON запиту мережевого шлюзу");
            replyJson = { {"status", "ERROR"}, {"message", "Критична помилка шлюзу"} };
        }

        string ser = replyJson.dump();
        string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=utf-8\r\nContent-Length: " + to_string(ser.length()) + "\r\nConnection: close\r\n\r\n" + ser;
        send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
    } else {
        string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: " + to_string(GUEST_WEB_PAGE.length()) + "\r\nConnection: close\r\n\r\n" + GUEST_WEB_PAGE;
        send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
    }
    closesocket(clientSocket);
}