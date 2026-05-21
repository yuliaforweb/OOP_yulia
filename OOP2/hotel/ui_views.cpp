#include "ui_shared.h"
#include "imgui.h"
#include "external/json.hpp"
#include "network_client.h"
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <fstream>

using namespace std;
using json = nlohmann::json;

// виділення пам'яті під усі глобальні змінні інтерфейсу оператора
bool isRequestProcessing = false;
string loginResponse = "";
string fundResponse = "";
string bookingResponse = "";
string passwordResponse = "";
string reportErrorResponse = "";

json reservationsArray = json::array();
json guestsArray = json::array();
json occupancyArray = json::array();
json financialData = json::object();
vector<RoomData> clientAvailableRooms;

int activeReportType = 0;
int activeTabTracker = 0;
int programmaticJumpTab = -1;

bool isLoggedIn = false;
string userRole = "Не визначено";
int userRoleGlobalIdx = 1;

int failedLoginAttempts = 0;
bool isInterfaceLocked = false;
chrono::steady_clock::time_point lockStartTime;
const int LOCKOUT_DURATION = 120;

char bufFundCheckIn[11] = "";
char bufFundCheckOut[11] = "";
char bufBookingLastName[41] = "";
char bufBookingFirstName[41] = "";
char bufBookingMiddleName[41] = "";
char bufBookingPhone[20] = "";
char bufBookingPassport[32] = "";
char bufBookingCheckIn[11] = "";
char bufBookingCheckOut[11] = "";
int chosenCategoryCombo = 0;
char bufSearchPassport[32] = "";
char bufReportStart[11] = "";
char bufReportEnd[11] = "";
char bufSecurityOldPass[64] = "";
char bufSecurityNewPass[64] = "";
char bufSecurityConfPass[64] = "";
int securityRoleIdx = 0;

void exportReportToHtml(const string& data, int mode) {
    ofstream f("print_report.html");
    if (!f.is_open()) return;
    f << "<html><head><meta charset='utf-8'><style>";
    f << "body { font-family: sans-serif; padding: 30px; color: #333; }";
    f << "table { width: 100%; border-collapse: collapse; margin-top: 20px; }";
    f << "th, td { border: 1px solid #ccc; padding: 12px; text-align: left; }";
    f << "th { background-color: #f4f4f4; }</style></head><body>";
    f << "<h2>Звіт інформаційної системи керування готелем</h2>";

    if (mode == 1) {
        f << "<h3>Матриця завантаженості номерного фонду</h3>";
        f << "<table><tr><th>Номер</th><th>Категорія</th><th>Поточний стан</th></tr>";
        try {
            json arr = json::parse(data);
            for (auto& item : arr) {
                f << "<tr><td>" << item.value("room", 0) << "</td><td>" << item.value("desc", "-") << "</td><td>" << item.value("state", "-") << "</td></tr>";
            }
        } catch (const exception& parseEx) {
            f << "<tr><td colspan='3'>Помилка обробки箋структури даних</td></tr>";
        }
        f << "</table>";
    } else {
        f << "<h3>Финансовий аналіз закритих ордерів (тільки проведені оплати)</h3>";
        try {
            json obj = json::parse(data);
            f << "<p><b>Загальний чистий дохід комплексу:</b> " << obj.value("total_income", 0.0) << " грн</p>";
            f << "<p><b>Кількість закритих у базі ордерів:</b> " << obj.value("total_count", 0) << " од.</p>";
        } catch (const exception& parseEx) {
            f << "<p>Помилка обробки фінансових обєктів</p>";
        }
    }
    f << "<script>window.print();</script></body></html>";
    f.close();

    #if defined(_WIN32) || defined(_WIN64)
        system("start print_report.html");
    #elif defined(__linux__)
        system("xdg-open print_report.html");
    #endif
}

string fixApostrophe(string str) {
    size_t pos = 0;
    while ((pos = str.find("&#x27;", pos)) != string::npos) {
        str.replace(pos, 6, "'");
        pos += 1;
    }
    return str;
}

void applyDateMask(char* buf) {
    string pure = "";
    for (int i = 0; buf[i]; i++) if (isdigit(buf[i])) pure += buf[i];
    if (pure.length() > 8) pure = pure.substr(0, 8);
    if (pure.length() >= 2) {
        int day = stoi(pure.substr(0, 2));
        if (day > 31) { pure[0] = '3'; pure[1] = '1'; }
        if (day == 0) { pure[0] = '0'; pure[1] = '1'; }
    }
    if (pure.length() >= 4) {
        int month = stoi(pure.substr(2, 2));
        if (month > 12) { pure[2] = '1'; pure[3] = '2'; }
        if (month == 0) { pure[2] = '0'; pure[3] = '1'; }
    }
    if (pure.length() == 8) {
        int year = stoi(pure.substr(4, 4));
        if (year < 2026) { pure[4] = '2'; pure[5] = '0'; pure[6] = '2'; pure[7] = '6'; }
        if (year > 2035) { pure[4] = '2'; pure[5] = '0'; pure[6] = '3'; pure[7] = '5'; }
    }
    string res = "";
    if (pure.length() > 0) res += pure.substr(0, min((int)pure.length(), 2));
    if (pure.length() > 2) res += "." + pure.substr(2, min((int)pure.length() - 2, 2));
    if (pure.length() > 4) res += "." + pure.substr(4);
    snprintf(buf, 11, "%s", res.c_str());
}

void applyPhoneMask(char* buf) {
    string rawInput = buf;
    string digits = "";
    for (char c : rawInput) if (isdigit(c)) digits += c;
    string pure = "";
    if (digits.length() >= 3 && digits.substr(0, 3) == "380") pure = digits.substr(3);
    else if (digits.length() >= 1 && digits[0] == '0') pure = digits.substr(1);
    else pure = digits;
    if (pure.length() > 9) pure = pure.substr(0, 9);
    string res = "+38 (0" + pure.substr(0, min((int)pure.length(), 2));
    if (pure.length() > 2) res += ") " + pure.substr(2, min((int)pure.length() - 2, 3));
    if (pure.length() > 5) res += "-" + pure.substr(5, min((int)pure.length() - 5, 2));
    if (pure.length() > 7) res += "-" + pure.substr(7);

    snprintf(buf, 20, "%s", res.c_str());
}

void applyPassportMask(char* buf) {
    string str = buf;
    if (str.empty()) return;
    bool isIdCard = isdigit((unsigned char)str[0]);
    int maxChars = isIdCard ? 9 : 8;
    int charCount = 0; size_t byteIdx = 0;
    while (byteIdx < str.length() && charCount < maxChars) {
        if ((unsigned char)str[byteIdx] <= 127) byteIdx += 1;
        else byteIdx += 2;
        charCount++;
    }
    buf[byteIdx] = '\0';
}

void applyModernCyberStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f; style.FrameRounding = 6.0f;
    style.ItemSpacing = ImVec2(16.0f, 22.0f); style.WindowPadding = ImVec2(25.0f, 25.0f);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.16f, 0.26f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_Border] = ImVec4(0.24f, 0.34f, 0.50f, 1.00f);
    colors[ImGuiCol_Text] = ImVec4(0.94f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.28f, 0.42f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.60f, 0.86f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.16f, 0.45f, 0.74f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.28f, 0.42f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.34f, 0.50f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.20f, 0.30f, 1.00f);
}

void sendJsonRequest(json req) {
    isRequestProcessing = true;
    string cmd = req["command"].get<string>();
    int maxAttempts = 3; bool success = false; string reply = "";

    for (int attempt = 1; attempt <= maxAttempts; attempt++) {
        try {
            if (cmd == "LOGIN") loginResponse = "Спроба підключення до сервера: " + to_string(attempt) + " з " + to_string(maxAttempts);
            reply = sendPostRequest(req.dump());
            success = true; break;
        } catch (const exception& connectError) {
            if (attempt < maxAttempts) this_thread::sleep_for(chrono::seconds(1));
        }
    }
    if (!success) {
        if (cmd == "LOGIN") loginResponse = "Помилка: Збій підключення після серії спроб";
        isRequestProcessing = false; return;
    }
    try {
        json replyJson = json::parse(reply);
        if (!replyJson.is_object()) { isRequestProcessing = false; return; }

        if (cmd == "LOGIN") {
            if (replyJson.contains("status") && replyJson["status"].is_string() && replyJson["status"].get<string>() == "OK") {
                isLoggedIn = true; userRole = replyJson.value("role", "Не визначено"); failedLoginAttempts = 0;
            } else {
                failedLoginAttempts++;
                if (failedLoginAttempts >= 3) { isInterfaceLocked = true; lockStartTime = chrono::steady_clock::now(); }
                loginResponse = replyJson.value("message", "Помилка авторизації");
            }
        }
        else if (cmd == "CHECK_FREE_ROOMS") {
            clientAvailableRooms.clear();
            if (replyJson.contains("rooms") && replyJson["rooms"].is_array()) {
                for (const auto& item : replyJson["rooms"]) {
                    if (!item.is_object()) continue;
                    RoomData r; r.id = item.value("id", 0); r.desc = item.value("desc", "Номер");
                    r.available_count = item.value("available_count", 0); r.price = item.value("price", 0.0);
                    clientAvailableRooms.push_back(r);
                }
            }
        }
        else if (cmd == "CHANGE_PASSWORD") passwordResponse = replyJson.value("message", "Збій");
        else if (cmd == "CREATE_BOOKING" || cmd == "CONFIRM_BOOKING" || cmd == "CANCEL_BOOKING" || cmd == "PROCESS_RECEPTION_PAYMENT") {
            bookingResponse = replyJson.value("message", "Збій");
            if (replyJson.value("status", "") == "OK") {
                json refReq = { {"command", "SHOW_RESERVATIONS"}, {"passport", string(bufSearchPassport)} };
                try {
                    string refReply = sendPostRequest(refReq.dump());
                    json refJson = json::parse(refReply);
                    if (refJson.contains("arr") && refJson["arr"].is_array()) {
                        reservationsArray = refJson["arr"];
                    }
                } catch (...) {}
            }
        }
        else if (cmd == "SHOW_RESERVATIONS") { if (replyJson.contains("arr") && replyJson["arr"].is_array()) reservationsArray = replyJson["arr"]; }
        else if (cmd == "SHOW_GUESTS") { if (replyJson.contains("arr") && replyJson["arr"].is_array()) guestsArray = replyJson["arr"]; }
        else if (cmd == "REPORT_OCCUPANCY") { if (replyJson.contains("arr") && replyJson["arr"].is_array()) occupancyArray = replyJson["arr"]; }
        else if (cmd == "REPORT_FINANCIAL") { if (replyJson.contains("data") && replyJson["data"].is_object()) financialData = replyJson["data"]; }
    } catch (const exception& parseError) {
        loginResponse = "Помилка: Невірний формат відповіді шлюзу";
    }
    isRequestProcessing = false;
}

void drawMainUI(ImGuiIO& io, ImGuiWindowFlags flags) {
    applyModernCyberStyle();

    if (!isLoggedIn) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
        ImGui::Begin("##LoginFullScreen", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

        ImVec2 box_size = ImVec2(580.0f, 360.0f);
        float posX = (io.DisplaySize.x - box_size.x) * 0.5f;
        float posY = (io.DisplaySize.y - box_size.y) * 0.5f;
        ImGui::SetCursorPos(ImVec2(posX < 0.0f ? 0.0f : posX, posY < 0.0f ? 0.0f : posY));

        ImGui::BeginChild("LoginBox", box_size, false, ImGuiWindowFlags_NoScrollbar);
        ImGui::BeginChild("LoginFields", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_NoScrollbar);

        ImGui::Dummy(ImVec2(0.0f, 15.0f));
        ImGui::TextColored(ImVec4(0.20f, 0.60f, 0.86f, 1.00f), "Вхід у робочу панель:");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        if (isInterfaceLocked) {
            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::seconds>(now - lockStartTime).count();
            int remaining = LOCKOUT_DURATION - elapsed;

            if (remaining <= 0) {
                isInterfaceLocked = false; failedLoginAttempts = 0; loginResponse = "Екран розблоковано";
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.35f, 0.35f, 1.0f));
                ImGui::TextWrapped("ДОСТУП БЛОКОВАНО: Спрацював захист від підбору паролів");
                ImGui::PopStyleColor();
                ImGui::Dummy(ImVec2(0.0f, 12.0f));
                ImGui::TextColored(ImVec4(0.25f, 0.75f, 0.45f, 1.00f), "Час до автоматичного розблокування: %d сек", remaining);
                ImGui::EndChild(); ImGui::EndChild(); ImGui::End(); return;
            }
        }

        static int selectedRoleIndex = 0;
        const char* roles[] = { "Адміністратор", "Менеджер", "Рецепція" };
        ImGui::Text("Посада:"); ImGui::Combo("##role", &selectedRoleIndex, roles, 3);
        userRoleGlobalIdx = selectedRoleIndex + 1;

        static char passBuffer[64] = "";
        ImGui::Text("Пароль:"); ImGui::InputText("##password", passBuffer, sizeof(passBuffer), ImGuiInputTextFlags_Password);
        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        if (!loginResponse.empty()) {
            ImVec4 col = (loginResponse.find("Помилка") != string::npos) ? ImVec4(0.95f, 0.35f, 0.35f, 1.0f) : ImVec4(0.25f, 0.75f, 0.45f, 1.00f);
            ImGui::TextWrapped("%s", loginResponse.c_str());
        }

        if (ImGui::Button("Увійти", ImVec2(130.0f, 36.0f))) {
            json req = { {"command", "LOGIN"}, {"role", userRoleGlobalIdx}, {"password", string(passBuffer)} };
            thread(sendJsonRequest, req).detach();
        }
        ImGui::EndChild(); ImGui::EndChild(); ImGui::End();
    }
    else {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
        ImGui::Begin("##AdminFullScreenWindow", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

        ImGui::Dummy(ImVec2(0.0f, 30.0f));
        ImGui::TextColored(ImVec4(0.20f, 0.60f, 0.86f, 1.00f), "ІНФОРМАЦІЙНА СИСТЕМА КЕРУВАННЯ ГОТЕЛЕМ");
        ImGui::SameLine(ImGui::GetWindowWidth() - 250.0f);
        ImGui::TextColored(ImVec4(0.94f, 0.76f, 0.18f, 1.00f), "Роль: %s", userRole.c_str());
        ImGui::Separator(); ImGui::Dummy(ImVec2(0.0f, 15.0f));

        float container_width = io.DisplaySize.x * 0.94f;
        ImGui::SetCursorPosX((io.DisplaySize.x - container_width) / 2.0f);
        ImGui::BeginChild("MainContentContainer", ImVec2(container_width, -1.0f), false, 0);

        if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_None)) {

            if (activeTabTracker == 0) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.60f, 0.86f, 1.00f));
            bool t0 = ImGui::BeginTabItem("Номерний фонд", nullptr, (programmaticJumpTab == 0) ? ImGuiTabItemFlags_SetSelected : 0);
            if (activeTabTracker == 0) ImGui::PopStyleColor();
            if (t0) { activeTabTracker = 0; programmaticJumpTab = -1; drawContentRoomFund(); ImGui::EndTabItem(); }

            if (userRoleGlobalIdx == 1 || userRoleGlobalIdx == 3) {
                if (activeTabTracker == 1) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.60f, 0.86f, 1.00f));
                bool t1 = ImGui::BeginTabItem("Оформлення заявки", nullptr, (programmaticJumpTab == 1) ? ImGuiTabItemFlags_SetSelected : 0);
                if (activeTabTracker == 1) ImGui::PopStyleColor();
                if (t1) { activeTabTracker = 1; programmaticJumpTab = -1; drawContentCreateBooking(); ImGui::EndTabItem(); }
            }

            if (activeTabTracker == 2) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.60f, 0.86f, 1.00f));
            bool t2 = ImGui::BeginTabItem("Керування резервами", nullptr, (programmaticJumpTab == 2) ? ImGuiTabItemFlags_SetSelected : 0);
            if (activeTabTracker == 2) ImGui::PopStyleColor();
            if (t2) {
                activeTabTracker = 2;
                programmaticJumpTab = -1;

                // ВБУДОВАНИЙ ВНУТРІШНІЙ ТАБ-БАР ДЛЯ РОЗДІЛЕННЯ ЕКРАНА НА ДВІ ЧАСТИНИ
                if (userRoleGlobalIdx == 3 || userRoleGlobalIdx == 1) {
                    if (ImGui::BeginTabBar("SubManageTabs", ImGuiTabBarFlags_None)) {
                        if (ImGui::BeginTabItem("Журнал замовлень та редагування")) {
                            drawContentManageReservations();
                            ImGui::EndTabItem();
                        }
                        if (ImGui::BeginTabItem("Касовий термінал (Оплата)")) {
                            drawContentActiveReservations();
                            ImGui::EndTabItem();
                        }
                        ImGui::EndTabBar();
                    }
                } else {
                    drawContentManageReservations();
                }

                ImGui::EndTabItem();
            }

            if (activeTabTracker == 3) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.60f, 0.86f, 1.00f));
            bool t3 = ImGui::BeginTabItem("База гостей", nullptr, (programmaticJumpTab == 3) ? ImGuiTabItemFlags_SetSelected : 0);
            if (activeTabTracker == 3) ImGui::PopStyleColor();
            if (t3) { activeTabTracker = 3; programmaticJumpTab = -1; drawContentGuestsDatabase(); ImGui::EndTabItem(); }

            if (userRoleGlobalIdx == 1 || userRoleGlobalIdx == 2) {
                if (activeTabTracker == 4) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.60f, 0.86f, 1.00f));
                bool t4 = ImGui::BeginTabItem("Аналітичні звіти", nullptr, (programmaticJumpTab == 4) ? ImGuiTabItemFlags_SetSelected : 0);
                if (activeTabTracker == 4) ImGui::PopStyleColor();
                if (t4) { activeTabTracker = 4; programmaticJumpTab = -1; drawContentAnalyticalReports(); ImGui::EndTabItem(); }
            }

            if (activeTabTracker == 5) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.60f, 0.86f, 1.00f));
            bool t5 = ImGui::BeginTabItem("Безпека системи", nullptr, (programmaticJumpTab == 5) ? ImGuiTabItemFlags_SetSelected : 0);
            if (activeTabTracker == 5) ImGui::PopStyleColor();
            if (t5) { activeTabTracker = 5; programmaticJumpTab = -1; drawContentSystemSecurity(); ImGui::EndTabItem(); }

            ImGui::EndTabBar();
        }
        ImGui::EndChild(); ImGui::End();
    }
}