#ifndef UI_SHARED_H
#define UI_SHARED_H

#include "imgui.h"
#include "external/json.hpp"
#include <string>
#include <vector>
#include <chrono>

struct RoomData {
    int id;
    std::string desc;
    int available_count;
    double price;
};

// Глобальні прапорці та змінні мережевого стану
extern bool isRequestProcessing;
extern std::string loginResponse;
extern std::string fundResponse;
extern std::string bookingResponse;
extern std::string passwordResponse;
extern std::string reportErrorResponse;

extern nlohmann::json reservationsArray;
extern nlohmann::json guestsArray;
extern nlohmann::json occupancyArray;
extern nlohmann::json financialData;
extern std::vector<RoomData> clientAvailableRooms;

extern int activeReportType; 
extern int activeTabTracker;
extern int programmaticJumpTab;

extern bool isLoggedIn;
extern std::string userRole;
extern int userRoleGlobalIdx;

extern int failedLoginAttempts;
extern bool isInterfaceLocked;
extern std::chrono::steady_clock::time_point lockStartTime;
extern const int LOCKOUT_DURATION;

// Спільні текстові буфери введення для взаємодії між табами
extern char bufFundCheckIn[11];
extern char bufFundCheckOut[11];
extern char bufBookingLastName[41];
extern char bufBookingFirstName[41];
extern char bufBookingMiddleName[41];
extern char bufBookingPhone[20];
extern char bufBookingPassport[32];
extern char bufBookingCheckIn[11];
extern char bufBookingCheckOut[11];
extern int chosenCategoryCombo;
extern char bufSearchPassport[32];
extern char bufReportStart[11];
extern char bufReportEnd[11];
extern char bufSecurityOldPass[64];
extern char bufSecurityNewPass[64];
extern char bufSecurityConfPass[64];
extern int securityRoleIdx;

// Допоміжні функції валідації та запитів
void applyDateMask(char* buf);
void applyPhoneMask(char* buf);
void applyPassportMask(char* buf);
std::string fixApostrophe(std::string str);
void sendJsonRequest(nlohmann::json req);
void exportReportToHtml(const std::string& data, int mode);

// Функції малювання вмісту конкретних вікон готелю
void drawContentRoomFund();
void drawContentCreateBooking();
void drawContentManageReservations();
void drawContentGuestsDatabase();
void drawContentActiveReservations(); // Додано прототип для фінансового термінала ресепшену
void drawContentAnalyticalReports();
void drawContentSystemSecurity();

#endif