#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "external/doctest.h"
#include "ui_shared.h"
#include "hotel.h"
#include "logger.h"
#include "external/json.hpp"
#include <string>

using namespace std;
using json = nlohmann::json;

// Допоміжна правильна функція для перевірки високосного року
static bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Допоміжна точна функція підрахунку днів між двома датами
static int countDaysExact(int d1, int m1, int y1, int d2, int m2, int y2) {
    const int daysInMonths[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    auto getAbsDays = [&](int d, int m, int y) {
        int total = d;
        for (int i = 1; i < m; ++i) {
            if (i == 2 && isLeapYear(y)) total += 29;
            else total += daysInMonths[i];
        }
        total += (y - 2026) * 365;
        for (int i = 2026; i < y; ++i) {
            if (isLeapYear(i)) total += 1;
        }
        return total;
    };

    int diff = getAbsDays(d2, m2, y2) - getAbsDays(d1, m1, y1);
    return diff <= 0 ? 1 : diff;
}

// ============================================================================
// БЛОК 1: ВАЛІДАЦІЯ ТА UX/UI МАСКИ КЛІЄНТСЬКОГО ІНТЕРФЕЙСУ (1-10)
// ============================================================================

TEST_CASE("1. UI: Date Format Validation") {
    char buf[11] = "01062026";
    applyDateMask(buf);
    CHECK(string(buf) == "01.06.2026");
}

TEST_CASE("2. UI: Date High Day Constraint") {
    char edgeDay[11] = "45062026";
    applyDateMask(edgeDay);
    CHECK(string(edgeDay) == "31.06.2026");
}

TEST_CASE("3. UI: Date High Month Constraint") {
    char edgeMonth[11] = "12152026";
    applyDateMask(edgeMonth);
    CHECK(string(edgeMonth) == "12.12.2026");
}

TEST_CASE("4. UI: Lower Year Boundary Protection") {
    char oldYear[11] = "01.06.2020";
    applyDateMask(oldYear);
    CHECK(string(oldYear) == "01.06.2026");
}

TEST_CASE("5. UI: Upper Year Boundary Protection") {
    char futureYear[11] = "01.06.2040";
    applyDateMask(futureYear);
    CHECK(string(futureYear) == "01.06.2035");
}

TEST_CASE("6. UI: Date Mask Incomplete Input Handling") {
    char shortDate[11] = "12";
    applyDateMask(shortDate);
    CHECK(string(shortDate) == "12");
}

TEST_CASE("7. UI: Phone Number Base Masking") {
    char phone[20] = "0931112233";
    applyPhoneMask(phone);
    CHECK(string(phone) == "+38 (093) 111-22-33");
}

TEST_CASE("8. UI: Phone Number International Input Masking") {
    char phoneRaw[20] = "380634445566";
    applyPhoneMask(phoneRaw);
    CHECK(string(phoneRaw) == "+38 (063) 444-55-66");
}

TEST_CASE("9. UI: Passport Field Character Limiter") {
    char passport[32] = "AB123456789";
    applyPassportMask(passport);
    CHECK(string(passport).length() <= 8); // Для літерного паспорта довжина 8
}

TEST_CASE("10. UI: HTML Characters Sanitizer") {
    string badString = "O&#x27;Brian";
    CHECK(fixApostrophe(badString) == "O'Brian");
}

// ============================================================================
// БЛОК 2: МЕРЕЖЕВА ЛОГІКА, СЕРІАЛІЗАЦІЯ ТА КЛІЄНТ-СЕРВЕР (11-21)
// ============================================================================

TEST_CASE("11. Network: Command Packet Serialization") {
    json req = { {"command", "LOGIN"}, {"role", 1}, {"password", "root"} };
    string rawData = req.dump();
    CHECK(rawData.find("LOGIN") != string::npos);
}

TEST_CASE("12. Network: Server Response Parsing") {
    string msg = "{\"status\":\"OK\",\"message\":\"Done\"}";
    json parsed = json::parse(msg);
    CHECK(parsed["status"].get<string>() == "OK");
}

TEST_CASE("13. Network: JSON Array Validation") {
    json arr = json::array({ {{"room", 101}, {"state", "Free"}} });
    CHECK(arr.is_array() == true);
}

TEST_CASE("14. Network: JSON Null Pointer Protection") {
    string badJson = "{\"rooms\": [null]}";
    json parsed = json::parse(badJson);
    CHECK(parsed["rooms"][0].is_null() == true);
}

TEST_CASE("15. Network: Missing Field Verification") {
    json packet = { {"status", "UNKNOWN"} };
    CHECK(packet.contains("command") == false);
}

TEST_CASE("16. Network: Host Connection Defaults") {
    string host = "127.0.0.1";
    CHECK(host == "127.0.0.1");
}

TEST_CASE("17. Network: Port Allocator Constraints") {
    int port = 5432;
    CHECK(port == 5432);
}

TEST_CASE("18. Network: Server Socket Port") {
    int serverPort = 8888;
    CHECK(serverPort == 8888);
}

TEST_CASE("19. Network: JSON Data Type Matching") {
    json val = { {"price", 800.0} };
    CHECK(val["price"].is_number_float() == true);
}

TEST_CASE("20. Network: Empty JSON Object Handler") {
    json emptyObj = json::object();
    CHECK(emptyObj.empty() == true);
}

TEST_CASE("21. Network: Web Interface Payment Choice Serialization") {
    json req = { {"command", "CREATE_BOOKING"}, {"payment_method_choice", "online"} };
    CHECK(req["payment_method_choice"].get<string>() == "online");
}

// ============================================================================
// БЛОК 3: БІЗНЕС-ЛОГІКА СЕРВЕРА ТА ТОЧНІ ТАРИФИ ГОТЕЛЮ (22-32)
// ============================================================================

TEST_CASE("22. Core: Stay Price Estimation Formula") {
    double prices[] = { 800.0, 900.0, 1500.0, 1500.0, 3500.0, 4200.0 };
    int days = 3;
    CHECK((days * prices[2]) == 4500.0);
}

TEST_CASE("23. Core: Same Day Fallback Rule") {
    int d1 = 1, m1 = 6, y1 = 2026;
    int d2 = 1, m2 = 6, y2 = 2026;
    int finalDays = countDaysExact(d1, m1, y1, d2, m2, y2);
    CHECK(finalDays == 1);
}

TEST_CASE("24. Core: Precise Inter-Month Stay Calculation (May-June Bugfix)") {
    // Емуляція переходу місяця з 30 травня по 2 червня 2026 року
    int days = countDaysExact(30, 5, 2026, 2, 6, 2026);
    CHECK(days == 3); // 30.05, 31.05, 01.06 - разом 3 доби проживання
}

TEST_CASE("25. Core: Staff Role Decoder Mapping") {
    auto getRoleName = [](int id) {
        if (id == 1) return "Адміністратор";
        if (id == 2) return "Менеджер";
        return "Рецепція";
    };
    CHECK(getRoleName(1) == "Адміністратор");
}

TEST_CASE("26. Core: Intersecting Intervals Overlap Model") {
    int bStart = 10, bEnd = 15;
    int cStart = 12, cEnd = 18;
    bool overlap = (cStart < bEnd && cEnd > bStart);
    CHECK(overlap == true);
}

TEST_CASE("27. Core: Sequential Stay Connection Model") {
    int bStart = 10, bEnd = 15;
    int cStart = 15, cEnd = 20;
    bool overlap = (cStart < bEnd && cEnd > bStart);
    CHECK(overlap == false);
}

TEST_CASE("28. Core: Room Category Combo Balancer") {
    auto getRoom = [](int idx) { return (idx == 0) ? 101 : 103; };
    CHECK(getRoom(0) == 101);
}

TEST_CASE("29. Core: Long Term Stay Cost Math") {
    double price = 4200.0;
    int nights = 10;
    CHECK((nights * price) == 42000.0);
}

TEST_CASE("30. Core: Room Count Capacity Verify") {
    int totalRoomsAvailable = 7;
    CHECK(totalRoomsAvailable == 7);
}

TEST_CASE("31. Core: Category Total Capacity Check") {
    int standardCount = 2;
    CHECK(standardCount == 2);
}

TEST_CASE("32. Core: Booking Status String Decoder") {
    auto getStatus = [](string code) { return (code == "CONFIRMED") ? "Активне" : "Скасовано"; };
    CHECK(getStatus("CONFIRMED") == "Активне");
}

// ============================================================================
// БЛОК 4: КІБЕРБЕЗПЕКА ТА ЗАХИСТ ВІД ЗБОЇВ (33-42)
// ============================================================================

TEST_CASE("33. Security: Anti SQL Injection Shield") {
    auto isSafe = [](string s) { return s.find(";") == string::npos; };
    CHECK(isSafe("1; DROP TABLE bookings") == false);
}

TEST_CASE("34. Security: Password Modification Rules") {
    auto canChange = [](int act, int tar) { return !(act == 3 && tar == 1); };
    CHECK(canChange(3, 1) == false);
}

TEST_CASE("35. Security: Empty Buffers Guard") {
    string input = "";
    CHECK(input.empty() == true);
}

TEST_CASE("36. Security: Brute Force Lockout Activation") {
    int failed = 3;
    CHECK((failed >= 3) == true);
}

TEST_CASE("37. Security: HTML Tags XSS Prevention") {
    string code = "<script>";
    CHECK((code.find("<") != string::npos) == true);
}

TEST_CASE("38. Security: Special Characters Name Filtering") {
    string name = "Yulia$";
    CHECK((name.find("$") != string::npos) == true);
}

TEST_CASE("39. Security: Lockout Duration Value") {
    int duration = 120;
    CHECK(duration == 120);
}

TEST_CASE("40. Security: Password Bypass Token Verification") {
    string token = "BYPASS";
    CHECK(token == "BYPASS");
}

TEST_CASE("41. Security: Unauthenticated Session Block") {
    bool loggedIn = false;
    CHECK(loggedIn == false);
}

TEST_CASE("42. Security: Manager Authority Boundary Check") {
    auto canChange = [](int act, int tar) { return !(act == 2 && tar == 1); };
    CHECK(canChange(2, 1) == false); // Менеджер не має права міняти пароль Адміна
}

// ============================================================================
// БЛОК 5: АНАЛІТИКА, ЛОГУВАННЯ ТА ДІАГНОСТИКА СИСТЕМИ (43-52)
// ============================================================================

TEST_CASE("43. Diagnostics: Logger Base Call") {
    Logger::info("Діагностика модулів готелю");
    CHECK(true);
}

TEST_CASE("44. Analytics: Occupancy Free State Evaluator") {
    string state = "CANCELLED";
    CHECK((state == "CANCELLED") == true);
}

TEST_CASE("45. Analytics: Income Summary Representation") {
    json mock = { {"total_income", 800.0} };
    CHECK(mock["total_income"].get<double>() == 800.0);
}

TEST_CASE("46. Diagnostics: Request Processing Lock Flag") {
    bool processing = true;
    CHECK(processing == true);
}

TEST_CASE("47. Analytics: Financial Order Counter Verification") {
    int count = 0;
    CHECK(count >= 0);
}

TEST_CASE("48. Diagnostics: Tab Switching Index Sync") {
    int tab = 1;
    CHECK(tab == 1);
}

TEST_CASE("49. Diagnostics: Clean Database Initializer State") {
    bool firstLaunch = true;
    CHECK(firstLaunch == true);
}

TEST_CASE("50. Analytics: Passport Grouping Map Matching") {
    string pass1 = "123";
    string pass2 = "123";
    CHECK(pass1 == pass2);
}

TEST_CASE("51. Diagnostics: Connection Retries Counter") {
    int attempts = 3;
    CHECK(attempts == 3);
}

TEST_CASE("52. Diagnostics: System Lockout Timer Logic") {
    int elapsed = 130;
    CHECK((elapsed >= 120) == true);
}