#include "imgui_listener.h"
#include "hotel.h"
#include "logger.h"
#include "external/json.hpp"
#include <string>
#include <iostream>

using namespace std;
using json = nlohmann::json;

void handleImGuiClient(SOCKET clientSocket, HotelSystem& hotel, const string& rawRequest) {
    // обробка внутрішніх запитів моніторингу адмінки готелю
    try {
        if (rawRequest.empty()) {
            closesocket(clientSocket);
            return;
        }

        json requestJson = json::parse(rawRequest);
        string cmd = requestJson.value("command", "");
        json replyJson;

        // логування кожної дії через централізований модуль логера
        Logger::info("Компонент imgui_listener прийняв команду: " + cmd);

        if (cmd == "REPORT_OCCUPANCY") {
            // витягування дати з пакета або встановлення дефолт поточного року
            string start = requestJson.value("start", "01.01.2026");
            string end = requestJson.value("end", "31.12.2026");

            // два обов'язкові аргументи дат у метод як вимагає hotel.h
            string dataStr = hotel.getOccupancyReportStr(start, end);
            replyJson = { {"status", "OK"}, {"arr", json::parse(dataStr)} };
        }
        else if (cmd == "REPORT_FINANCIAL") {
            // аналогічні рамки періоду для фінансової аналітики доходу
            string start = requestJson.value("start", "01.01.2026");
            string end = requestJson.value("end", "31.12.2026");

            // виклик функції ядра готелю под нову сигнатуру
            string dataStr = hotel.getFinancialReportStr(start, end);
            replyJson = { {"status", "OK"}, {"data", json::parse(dataStr)} };
        }
        else if (cmd == "SHOW_RESERVATIONS") {
            string passport = requestJson.value("passport", "");
            string dataStr = hotel.getReservationsStr(passport);
            replyJson = { {"status", "OK"}, {"arr", json::parse(dataStr)} };
        }
        else if (cmd == "SHOW_GUESTS") {
            string dataStr = hotel.getGuestsStr();
            replyJson = { {"status", "OK"}, {"arr", json::parse(dataStr)} };
        }
        // --- НОВИЙ БЛОК: ОБРОБКА ПРОВЕДЕННЯ ОПЛАТИ НА РЕСЕПШЕНІ ---
        else if (cmd == "PROCESS_RECEPTION_PAYMENT") {
            int bId = requestJson.value("booking_id", 0);
            string method = requestJson.value("payment_method", "cash");

            bool ok = hotel.processReceptionPayment(bId, method);
            replyJson = {
                {"status", ok ? "OK" : "ERROR"},
                {"message", ok ? "Оплату успішно зафіксовано в системі" : "Збій проведення транзакції банку"}
            };
        }
        // --- НОВИЙ БЛОК: ОБРОБКА РЕДАГУВАННЯ КАРТКИ ЗАМОВЛЕННЯ ---
        else if (cmd == "UPDATE_BOOKING_DATA") {
            int bId = requestJson.value("id", 0);
            string name = requestJson.value("fullname", "");
            string phone = requestJson.value("phone", "");
            string pass = requestJson.value("passport", "");
            string inDate = requestJson.value("checkin", "");
            string outDate = requestJson.value("checkout", "");

            // Виклик методу ядра для оновлення PostgreSQL
            bool ok = hotel.updateBookingData(bId, name, phone, pass, inDate, outDate);
            replyJson = {
                {"status", ok ? "OK" : "ERROR"},
                {"message", ok ? "Дані успішно змінено!" : "Помилка оновлення таблиць PostgreSQL"}
            };
        }
        // --- НОВИЙ БЛОК: ОБРОБКА ЕКСПОРТУ У ФОРМАТ EXCEL (XLS) ---
        else if (cmd == "EXPORT_PAYMENTS_XLS") {
            // Зберігаємо файл у корінь робочої директорії збірки CLion
            bool ok = hotel.exportPaymentsToExcel("payments_report.xls");
            replyJson = {
                {"status", ok ? "OK" : "ERROR"},
                {"message", ok ? "Звіт успішно збережено у файл payments_report.xls" : "Не вдалося згенерувати XLS файл"}
            };
        }
        else {
            replyJson = { {"status", "ERROR"}, {"message", "Невідома внутрішня команда моніторингу"} };
        }

        string responseStr = replyJson.dump();
        send(clientSocket, responseStr.c_str(), responseStr.size(), 0);
    }
    catch (const exception& e) {
        Logger::error("Виник критичний збій обробника ланки моніторингу imgui");
    }

    // обов'язкове закриття сокета після відправки відповіді клiєнту
    closesocket(clientSocket);
}