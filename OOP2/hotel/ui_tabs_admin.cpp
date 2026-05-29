#include "ui_shared.h"
#include "imgui.h"
#include "external/json.hpp"
#include <string>
#include <thread>
#include <map>
#include <cstdio>
#include <cstring>
#include <algorithm>

using namespace std;
using json = nlohmann::json;

static map<int, int> selectedReceptionPayMethod;

void drawContentGuestsDatabase() {
    ImGui::Dummy(ImVec2(0.0f, 12.0f));
    if (ImGui::Button("Синхронізувати анкетні дані")) {
        json req = { {"command", "SHOW_GUESTS"} };
        thread(sendJsonRequest, req).detach();
    }
    ImGui::Dummy(ImVec2(0.0f, 12.0f));

    if (ImGui::BeginTable("GuestsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("ID"); ImGui::TableSetupColumn("Повне ім'я (ПІБ)"); ImGui::TableSetupColumn("Телефон");
        ImGui::TableSetupColumn("Паспорт");
        ImGui::TableHeadersRow();

        if (!guestsArray.empty() && guestsArray.is_array()) {
            map<string, json> groupedGuests;
            for (auto& item : guestsArray) {
                if (!item.is_object() || item.is_null()) continue;
                string pass = item.value("passport", "");
                if (groupedGuests.find(pass) == groupedGuests.end()) {
                    groupedGuests[pass] = item;
                }
            }

            for (auto& pair : groupedGuests) {
                json guest = pair.second;
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%d", guest.value("id", 0));
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", fixApostrophe(guest.value("fullname", "Невідомо")).c_str());
                ImGui::TableSetColumnIndex(2); ImGui::Text("%s", guest.value("phone", "-").c_str());
                ImGui::TableSetColumnIndex(3); ImGui::Text("%s", guest.value("passport", "-").c_str());
            }
        }
        ImGui::EndTable();
    }
}

void drawContentActiveReservations() {
    ImGui::Dummy(ImVec2(0.0f, 12.0f));
    if (ImGui::Button("Оновити список бронювань")) {
        json req = { {"command", "SHOW_RESERVATIONS"} };
        thread(sendJsonRequest, req).detach();
    }
    ImGui::Dummy(ImVec2(0.0f, 12.0f));

    if (ImGui::BeginTable("ReservationsTable", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Гість");
        ImGui::TableSetupColumn("Номер");
        ImGui::TableSetupColumn("Період");
        ImGui::TableSetupColumn("Сума");
        ImGui::TableSetupColumn("Тип оплати");
        ImGui::TableSetupColumn("Статус оплати");
        ImGui::TableSetupColumn("Дія за заселенням");
        ImGui::TableHeadersRow();

        if (!reservationsArray.empty() && reservationsArray.is_array()) {
            for (auto& item : reservationsArray) {
                if (!item.is_object() || item.is_null()) continue;

                int bookingId = item.value("id", 0);
                string payMethod = item.value("payment_method", "none");
                string payStatus = item.value("payment_status", "pending");
                string status = item.value("status", "UNKNOWN");
                transform(status.begin(), status.end(), status.begin(), ::toupper);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%d", bookingId);
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", fixApostrophe(item.value("fullname", "Невідомо")).c_str());
                ImGui::TableSetColumnIndex(2); ImGui::Text("%d", item.value("room", 0));
                ImGui::TableSetColumnIndex(3); ImGui::Text("%s - %s", item.value("checkin", "").c_str(), item.value("checkout", "").c_str());
                ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f грн", item.value("total_pay", 0.0));

                ImGui::TableSetColumnIndex(5);
                if (payMethod == "online") ImGui::Text("Онлайн");
                else if (payMethod == "cash") ImGui::Text("Готівка");
                else ImGui::Text("Не вказано");

                ImGui::TableSetColumnIndex(6);
                if (payStatus == "paid") {
                    ImGui::TextColored(ImVec4(0.25f, 0.75f, 0.45f, 1.0f), "Оплачено");
                } else if (payStatus == "upon_check_in") {
                    ImGui::TextColored(ImVec4(0.95f, 0.65f, 0.25f, 1.0f), "При заселенні");
                } else {
                    ImGui::TextColored(ImVec4(0.95f, 0.35f, 0.35f, 1.0f), "Очікує");
                }

                ImGui::TableSetColumnIndex(7);
                if (status == "CANCELLED") {
                    ImGui::TextColored(ImVec4(0.95f, 0.35f, 0.35f, 1.0f), "Скасовано");
                } else if (payStatus != "paid") {
                    if (selectedReceptionPayMethod.find(bookingId) == selectedReceptionPayMethod.end()) {
                        selectedReceptionPayMethod[bookingId] = 0;
                    }

                    ImGui::PushID(bookingId);
                    ImGui::RadioButton("Готівка", &selectedReceptionPayMethod[bookingId], 0); ImGui::SameLine();
                    ImGui::RadioButton("Термінал", &selectedReceptionPayMethod[bookingId], 1); ImGui::SameLine();

                    if (ImGui::Button("Провести Оплату")) {
                        string selectedStr = (selectedReceptionPayMethod[bookingId] == 0) ? "cash" : "online";
                        json req = {
                            {"command", "PROCESS_PAYMENT"},
                            {"id", bookingId},
                            {"method", selectedStr}
                        };
                        thread(sendJsonRequest, req).detach();
                    }
                    ImGui::PopID();
                } else {
                    ImGui::TextColored(ImVec4(0.25f, 0.75f, 0.45f, 1.0f), "Розраховано");
                }
            }
        }
        ImGui::EndTable();
    }
}

void drawContentAnalyticalReports() {
    ImGui::Dummy(ImVec2(0.0f, 12.0f));

    ImGui::Text("З дати:"); ImGui::SameLine(); ImGui::PushItemWidth(110.0f);
    ImGui::InputTextWithHint("##rs", "дд.мм.рррр", bufReportStart, 11); applyDateMask(bufReportStart); ImGui::PopItemWidth();
    ImGui::SameLine(); ImGui::Text("  По дату:"); ImGui::SameLine(); ImGui::PushItemWidth(110.0f);
    ImGui::InputTextWithHint("##re", "дд.мм.рррр", bufReportEnd, 11); applyDateMask(bufReportEnd); ImGui::PopItemWidth();

    int fd1 = 0, fm1 = 0, fy1 = 0, fd2 = 0, fm2 = 0, fy2 = 0;
    sscanf(bufReportStart, "%d.%d.%d", &fd1, &fm1, &fy1);
    sscanf(bufReportEnd, "%d.%d.%d", &fd2, &fm2, &fy2);
    int reportDays = (fd2 + fm2 * 30 + fy2 * 365) - (fd1 + fm1 * 30 + fy1 * 365);
    bool isReportDateInvalid = (reportDays <= 0 && strlen(bufReportStart) == 10 && strlen(bufReportEnd) == 10);

    if (isReportDateInvalid || !reportErrorResponse.empty()) {
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::TextColored(ImVec4(0.95f, 0.35f, 0.35f, 1.0f), "%s", isReportDateInvalid ? "Помилка: Дата початку не може бути рівною або пізнішою за дату завершення!" : reportErrorResponse.c_str());
    }

    ImGui::Dummy(ImVec2(0.0f, 4.0f));
    if (ImGui::Button("Сформувати матрицю завантаженості")) {
        if (string(bufReportStart).empty() || string(bufReportEnd).empty()) {
            reportErrorResponse = "Помилка: Вкажіть проміжок дат для звіту завантаженості!";
        } else {
            reportErrorResponse = "";
            activeReportType = 1;
            json req = { {"command", "REPORT_OCCUPANCY"}, {"start", string(bufReportStart)}, {"end", string(bufReportEnd)} };
            thread(sendJsonRequest, req).detach();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Финансовий аналіз доходу")) {
        if (string(bufReportStart).empty() || string(bufReportEnd).empty()) {
            reportErrorResponse = "Помилка: Вкажіть проміжок дат для фінансового аналізу!";
        } else {
            reportErrorResponse = "";
            activeReportType = 2;
            json req = { {"command", "REPORT_FINANCIAL"}, {"start", string(bufReportStart)}, {"end", string(bufReportEnd)} };
            thread(sendJsonRequest, req).detach();
        }
    }
    ImGui::Dummy(ImVec2(0.0f, 12.0f));

    if (activeReportType == 1) {
        if (ImGui::BeginTable("OccTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Номер"); ImGui::TableSetupColumn("Опис категорії"); ImGui::TableSetupColumn("Стан");
            ImGui::TableHeadersRow();
            if (!occupancyArray.empty() && occupancyArray.is_array()) {
                for (auto& item : occupancyArray) {
                    if (!item.is_object() || item.is_null()) continue;
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("%d", item.value("room", 0));
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%s", item.value("desc", "Без опису").c_str());
                    ImGui::TableSetColumnIndex(2); ImGui::Text("%s", item.value("state", "Вільний").c_str());
                }
            }
            ImGui::EndTable();
        }
    }
    else if (activeReportType == 2) {
        if (!financialData.empty() && financialData.is_object() && !financialData.is_null()) {
            string startPeriod = financialData.value("report_period_start", "-");
            string endPeriod = financialData.value("report_period_end", "-");

            ImGui::TextColored(ImVec4(0.94f, 0.76f, 0.18f, 1.00f), "ЗВІТ ЗА ПЕРІОД ПРОЖИВАННЯ: %s - %s", startPeriod.c_str(), endPeriod.c_str());
            ImGui::Dummy(ImVec2(0.0f, 4.0f));

            ImGui::TextColored(ImVec4(0.25f, 0.75f, 0.45f, 1.00f), "Фінансові результати за обраний період проживання (тільки проведені оплати):");
            ImGui::Text("Загальний чистий дохід комплексу: %.2f грн", financialData.value("total_income", 0.0));

            if (financialData.contains("list") && financialData["list"].is_array() && !financialData["list"].is_null()) {
                ImGui::Dummy(ImVec2(0.0f, 6.0f));
                if (ImGui::BeginTable("FinDetailsTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
                    ImGui::TableSetupColumn("ID"); ImGui::TableSetupColumn("Номер"); ImGui::TableSetupColumn("Гість");
                    ImGui::TableSetupColumn("Сума"); ImGui::TableSetupColumn("Дата оплати");
                    ImGui::TableHeadersRow();

                    for (auto& row : financialData["list"]) {
                        if (row.is_null() || !row.is_object()) continue;
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("%d", row.value("id", 0));
                        ImGui::TableSetColumnIndex(1); ImGui::Text("%d", row.value("room", 0));
                        ImGui::TableSetColumnIndex(2); ImGui::Text("%s", fixApostrophe(row.value("guest", "-")).c_str());
                        ImGui::TableSetColumnIndex(3); ImGui::Text("%.2f грн", row.value("amount", 0.0));
                        ImGui::TableSetColumnIndex(4); ImGui::Text("%s", row.value("payment_date", "-").c_str());
                    }
                    ImGui::EndTable();
                }
            }
        }
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    if (activeReportType != 0) {
        bool isReportEmpty = false;
        if (activeReportType == 1 && occupancyArray.empty()) isReportEmpty = true;
        if (activeReportType == 2 && (!financialData.contains("list") || financialData["list"].empty())) isReportEmpty = true;

        if (isReportEmpty) ImGui::BeginDisabled();
        if (ImGui::Button("Експорт звіту в веб-браузер (HTML)", ImVec2(260.0f, 40.0f))) {
            json req = { {"command", "EXPORT_PAYMENTS_XLS"} };
            thread(sendJsonRequest, req).detach();
        }
        if (isReportEmpty) ImGui::EndDisabled();
    }
}

void drawContentSystemSecurity() {
    ImGui::Dummy(ImVec2(0.0f, 12.0f));
    ImGui::Text("Зміна секретного авторизаційного коду:");

    const char* chRoles[] = { "Адміністратор", "Менеджер", "Рецепція" };
    ImGui::Text("Виберіть посаду для зміни:");
    ImGui::Combo("##chRole", &securityRoleIdx, chRoles, 3);
    int targetRole = securityRoleIdx + 1;

    if (targetRole == userRoleGlobalIdx) {
        ImGui::Text("Введіть поточний пароль посади:");
        ImGui::InputText("##oldPass", bufSecurityOldPass, sizeof(bufSecurityOldPass), ImGuiInputTextFlags_Password);
    }

    ImGui::Text("Введіть новий таємний пароль:");
    ImGui::InputText("##newPass", bufSecurityNewPass, sizeof(bufSecurityNewPass), ImGuiInputTextFlags_Password);

    ImGui::Text("Повторіть новий таємний пароль:");
    ImGui::InputText("##confPass", bufSecurityConfPass, sizeof(bufSecurityConfPass), ImGuiInputTextFlags_Password);
    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    if (!passwordResponse.empty()) {
        ImGui::TextWrapped("%s", passwordResponse.c_str());
    }

    if (ImGui::Button("Оновити пароль посади", ImVec2(220.0f, 44.0f))) {
        string oldStr = string(bufSecurityOldPass);
        string newStr = string(bufSecurityNewPass);
        string confStr = string(bufSecurityConfPass);

        if (targetRole == userRoleGlobalIdx && oldStr.empty()) {
            passwordResponse = "Помилка: Необхідно ввести поточний пароль для підтвердження особи!";
        } else if (newStr.empty() || confStr.empty()) {
            passwordResponse = "Помилка: Поля нового пароля не можуть бути порожніми!";
        } else if (newStr == oldStr && targetRole == userRoleGlobalIdx) {
            passwordResponse = "Помилка: Новий таємний код збігається з поточним паролем!";
        } else if (newStr != confStr) {
            passwordResponse = "Помилка: Нові паролі не збігаються між собою!";
        }
        else if (userRoleGlobalIdx == 2 && targetRole == 1) {
            passwordResponse = "Помилка: Менеджер не має прав змінювати пароль Адміністратора!";
        }
        else if (userRoleGlobalIdx == 3 && (targetRole == 1 || targetRole == 2)) {
            passwordResponse = "Помилка: Рецепція не має прав змінювати пароль Менеджера або Адміністратора!";
        }
        else {
            json req = {
                {"command", "CHANGE_PASSWORD"},
                {"role", targetRole},
                {"current_role", userRoleGlobalIdx},
                {"old_password", (targetRole == userRoleGlobalIdx ? oldStr : "BYPASS")},
                {"new_password", newStr}
            };
            thread(sendJsonRequest, req).detach();
        }
    }
}