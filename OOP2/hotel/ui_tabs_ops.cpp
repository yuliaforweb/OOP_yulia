#include "ui_shared.h"
#include "imgui.h"
#include "external/json.hpp"
#include "network_client.h"
#include <string>
#include <thread>
#include <vector>
#include <cstdio>
#include <cstring>

using namespace std;
using json = nlohmann::json;

static int bookingIdToCancel = -1;
static int editBookingId = -1;
static char editBufName[128] = "";
static char editBufPhone[32] = "";
static char editBufPass[32] = "";
static char editBufIn[11] = "";
static char editBufOut[11] = "";
static int guestPaymentChoice = 0;

void drawContentRoomFund() {
    ImGui::Dummy(ImVec2(0.0f, 12.0f));
    ImGui::Text("Дата заїзду:"); ImGui::SameLine(); ImGui::PushItemWidth(110.0f);
    ImGui::InputTextWithHint("##ni", "дд.мм.рррр", bufFundCheckIn, 11); applyDateMask(bufFundCheckIn); ImGui::PopItemWidth();

    ImGui::SameLine(); ImGui::Text("  Дата виїзду:"); ImGui::SameLine(); ImGui::PushItemWidth(110.0f);
    ImGui::InputTextWithHint("##no", "дд.мм.рррр", bufFundCheckOut, 11); applyDateMask(bufFundCheckOut); ImGui::PopItemWidth();

    int fd1 = 0, fm1 = 0, fy1 = 0, fd2 = 0, fm2 = 0, fy2 = 0;
    sscanf(bufFundCheckIn, "%d.%d.%d", &fd1, &fm1, &fy1);
    sscanf(bufFundCheckOut, "%d.%d.%d", &fd2, &fm2, &fy2);
    int fundDays = (fd2 + fm2 * 30 + fy2 * 365) - (fd1 + fm1 * 30 + fy1 * 365);
    bool isFundDateInvalid = (fundDays <= 0 && strlen(bufFundCheckIn) == 10 && strlen(bufFundCheckOut) == 10);

    ImGui::SameLine();
    if (ImGui::Button("Пошук вільних місць")) {
        if (isFundDateInvalid) {
            fundResponse = "Помилка: Дата заїзду не може бути рівною або пізнішою за дату виїзду!";
        } else {
            fundResponse = "";
            json req = { {"command", "CHECK_FREE_ROOMS"}, {"checkin", string(bufFundCheckIn)}, {"checkout", string(bufFundCheckOut)} };
            thread(sendJsonRequest, req).detach();
        }
    }

    if (isFundDateInvalid || !fundResponse.empty()) {
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::TextColored(ImVec4(0.95f, 0.35f, 0.35f, 1.0f), "%s", isFundDateInvalid ? "Помилка: Некоректний діапазон дат для пошуку номерів!" : fundResponse.c_str());
    }

    ImGui::Dummy(ImVec2(0.0f, 12.0f));
    if (ImGui::BeginTable("FundTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Категорія номеру комплексу"); ImGui::TableSetupColumn("В наявності"); ImGui::TableSetupColumn("Ціна за добу"); ImGui::TableSetupColumn("Дія");
        ImGui::TableHeadersRow();
        for (size_t i = 0; i < clientAvailableRooms.size(); i++) {
            auto& row = clientAvailableRooms[i];
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", row.desc.c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%d ном.", row.available_count);
            ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f грн", row.price);
            ImGui::TableSetColumnIndex(3);

            ImGui::PushID(static_cast<int>(i));
            bool isUnavailable = (row.available_count <= 0);
            if (isUnavailable) ImGui::BeginDisabled();

            if (ImGui::Button("Обрати")) {
                snprintf(bufBookingCheckIn, 11, "%s", bufFundCheckIn);
                snprintf(bufBookingCheckOut, 11, "%s", bufFundCheckOut);
                chosenCategoryCombo = static_cast<int>(i);
                programmaticJumpTab = 1;
            }

            if (isUnavailable) ImGui::EndDisabled();
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void drawContentCreateBooking() {
    ImGui::Dummy(ImVec2(0.0f, 12.0f));
    ImGui::Text("Прізвище:"); ImGui::InputTextWithHint("##ln", "Обов'язкове поле", bufBookingLastName, 41);
    ImGui::Text("Ім'я:"); ImGui::InputTextWithHint("##fn", "Обов'язкове поле", bufBookingFirstName, 41);
    ImGui::Text("По батькові:"); ImGui::InputTextWithHint("##mn", "Обов'язкове поле", bufBookingMiddleName, 41);
    ImGui::Text("Телефон:"); if (ImGui::InputTextWithHint("##ph", "+380XXXXXXXXX", bufBookingPhone, 20)) applyPhoneMask(bufBookingPhone);
    ImGui::Text("Паспорт:"); if (ImGui::InputTextWithHint("##pass", "Номер документа", bufBookingPassport, 32)) applyPassportMask(bufBookingPassport);
    ImGui::Text("Дата заїзду:"); if (ImGui::InputTextWithHint("##ci", "дд.мм.рррр", bufBookingCheckIn, 11)) applyDateMask(bufBookingCheckIn);
    ImGui::Text("Дата виїзду:"); if (ImGui::InputTextWithHint("##co", "дд.мм.рррр", bufBookingCheckOut, 11)) applyDateMask(bufBookingCheckOut);

    const char* categoriesList[] = {
        "Стандартний 2-місний (800 грн/доба)",
        "Стандартний 3-місний (900 грн/доба)",
        "Делюкс 2-місний з балконом (1500 грн/доба)",
        "Делюкс 3-місний без балкона (1500 грн/доба)",
        "Люкс 4-місний (3500 грн/доба)",
        "Люкс 6-місний (4200 грн/доба)"
    };
    double pricesList[] = { 800.0, 900.0, 1500.0, 1500.0, 3500.0, 4200.0 };

    ImGui::Text("Категорія номеру для заселення:");
    ImGui::Combo("##chosenCat", &chosenCategoryCombo, categoriesList, 6);

    int d1 = 0, m1 = 0, y1 = 0, d2 = 0, m2 = 0, y2 = 0;
    sscanf(bufBookingCheckIn, "%d.%d.%d", &d1, &m1, &y1);
    sscanf(bufBookingCheckOut, "%d.%d.%d", &d2, &m2, &y2);
    int daysCount = (d2 + m2 * 30 + y2 * 365) - (d1 + m1 * 30 + y1 * 365);

    bool isPeriodInvalid = (daysCount <= 0 && strlen(bufBookingCheckIn) == 10 && strlen(bufBookingCheckOut) == 10);
    bool datesMatchFund = (strcmp(bufBookingCheckIn, bufFundCheckIn) == 0 && strcmp(bufBookingCheckOut, bufFundCheckOut) == 0);
    bool isCategoryFull = false;

    if (datesMatchFund && chosenCategoryCombo < (int)clientAvailableRooms.size()) {
        if (clientAvailableRooms[chosenCategoryCombo].available_count <= 0) isCategoryFull = true;
    }

    if (isPeriodInvalid) {
        ImGui::TextColored(ImVec4(0.95f, 0.35f, 0.35f, 1.0f), "Помилка: Дата заїзду не може бути рівною або пізнішою за дату виїзду!");
    } else if (isCategoryFull) {
        ImGui::TextColored(ImVec4(0.95f, 0.35f, 0.35f, 1.0f), "Помилка: Усі номери цієї категорії на обрані дати вже зайняті!");
    } else {
        int finalDays = (daysCount <= 0) ? 1 : daysCount;
        double finalEstimatedCost = finalDays * pricesList[chosenCategoryCombo];
        ImGui::TextColored(ImVec4(0.25f, 0.75f, 0.45f, 1.00f), "Розрахункова тривалість: %d діб. Разом до сплати: %.2f грн", finalDays, finalEstimatedCost);
    }

    ImGui::Dummy(ImVec2(0.0f, 6.0f));
    ImGui::Separator();
    ImGui::Text("Опція оплати при оформленні:");
    ImGui::RadioButton("Клієнт сплачує аванс онлайн зараз (Імітація банку)", &guestPaymentChoice, 0);
    ImGui::RadioButton("Відкладена оплата (Розрахунок гостя при заселенні)", &guestPaymentChoice, 1);
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 6.0f));

    if (!bookingResponse.empty()) {
        ImVec4 col = (bookingResponse.find("Помилка") != string::npos) ? ImVec4(0.95f, 0.35f, 0.35f, 1.0f) : ImVec4(0.25f, 0.75f, 0.45f, 1.00f);
        ImGui::TextColored(col, "%s", bookingResponse.c_str());
    }

    if (isRequestProcessing) {
        ImGui::BeginDisabled();
        ImGui::Button("Обробка запиту...", ImVec2(280.0f, 46.0f));
        ImGui::EndDisabled();
    } else {
        if (isCategoryFull) ImGui::BeginDisabled();

        if (ImGui::Button("Внести в базу даних готелю", ImVec2(280.0f, 46.0f))) {
            string lName = string(bufBookingLastName); string fName = string(bufBookingFirstName); string mName = string(bufBookingMiddleName);
            string phoneStr = string(bufBookingPhone); string passportStr = string(bufBookingPassport);
            string ciStr = string(bufBookingCheckIn); string coStr = string(bufBookingCheckOut);

            auto hasBadSymbols = [](const string& s) {
                for (char c : s) {
                    if (string("=/\\[]|+=*<>?`~!@#$%^&*()_{};:\x22").find(c) != string::npos) return true;
                }
                return false;
            };

            if (lName.empty() || fName.empty() || mName.empty() || phoneStr.empty() || passportStr.empty() || ciStr.empty() || coStr.empty()) {
                bookingResponse = "Помилка: Усі без винятку поля форми є обов'язковими до введення!";
            } else if (daysCount <= 0) {
                bookingResponse = "Помилка: Заборонено бронювання! Дата заїзду повинна передувати даті виїзду.";
            } else if (hasBadSymbols(lName) || hasBadSymbols(fName) || hasBadSymbols(mName)) {
                bookingResponse = "Помилка: Поля ПІБ містьять заборонені спецсимволи чи цифри!";
            } else {
                string fullCombinedName = lName + " " + fName + " " + mName;

                int simulatedRoom = 101;
                if (chosenCategoryCombo == 0) simulatedRoom = 101;
                else if (chosenCategoryCombo == 1) simulatedRoom = 103;
                else if (chosenCategoryCombo == 2) simulatedRoom = 201;
                else if (chosenCategoryCombo == 3) simulatedRoom = 202;
                else if (chosenCategoryCombo == 4) simulatedRoom = 301;
                else if (chosenCategoryCombo == 5) simulatedRoom = 302;

                string payMethodParam = (guestPaymentChoice == 0) ? "online" : "upon_check_in";

                json req = {
                    {"command", "CREATE_BOOKING"}, {"guest_name", fullCombinedName}, {"phone", phoneStr},
                    {"passport", passportStr}, {"check_in", ciStr}, {"check_out", coStr}, {"room_number", simulatedRoom},
                    {"payment_method_choice", payMethodParam}
                };

                isRequestProcessing = true;
                thread(sendJsonRequest, req).detach();
            }
        }
        if (isCategoryFull) ImGui::EndDisabled();
    }
}

void drawContentManageReservations() {
    ImGui::Dummy(ImVec2(0.0f, 12.0f));
    ImGui::Text("Пошук за паспортом:"); ImGui::SameLine(); ImGui::InputText("##sPass", bufSearchPassport, 32); ImGui::SameLine();
    if (ImGui::Button("Оновити список")) {
        json req = { {"command", "SHOW_RESERVATIONS"}, {"passport_filter", string(bufSearchPassport)} };
        thread(sendJsonRequest, req).detach();
    }
    ImGui::Dummy(ImVec2(0.0f, 12.0f));

    static bool openCancelModalWindow = false;
    static bool openEditModalWindow = false;

    if (ImGui::BeginTable("ResTable", 12, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("ID"); ImGui::TableSetupColumn("Гість"); ImGui::TableSetupColumn("Паспорт");
        ImGui::TableSetupColumn("Телефон"); ImGui::TableSetupColumn("Номер"); ImGui::TableSetupColumn("Заїзд");
        ImGui::TableSetupColumn("Виїзд"); ImGui::TableSetupColumn("Вартість");
        ImGui::TableSetupColumn("Тип оплати"); ImGui::TableSetupColumn("Статус оплати"); ImGui::TableSetupColumn("Статус");
        ImGui::TableSetupColumn("Керування ордером");
        ImGui::TableHeadersRow();

        if (!reservationsArray.empty() && reservationsArray.is_array()) {
            for (auto& item : reservationsArray) {
                if (!item.is_object()) continue;
                int bId = item.value("id", 0);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%d", bId);
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", fixApostrophe(item.value("fullname", "Невідомо")).c_str());
                ImGui::TableSetColumnIndex(2); ImGui::Text("%s", item.value("passport", "-").c_str());
                ImGui::TableSetColumnIndex(3); ImGui::Text("%s", item.value("phone", "-").c_str());
                ImGui::TableSetColumnIndex(4); ImGui::Text("%d", item.value("room", 0));
                ImGui::TableSetColumnIndex(5); ImGui::Text("%s", item.value("checkin", "-").c_str());
                ImGui::TableSetColumnIndex(6); ImGui::Text("%s", item.value("checkout", "-").c_str());
                ImGui::TableSetColumnIndex(7); ImGui::Text("%.2f грн", item.value("total_pay", 0.0));

                ImGui::TableSetColumnIndex(8);
                string payMethod = item.value("payment_method", "none");
                if (payMethod == "online") ImGui::Text("Онлайн");
                else if (payMethod == "cash") ImGui::Text("Готівка");
                else ImGui::Text("Не вказано");

                ImGui::TableSetColumnIndex(9);
                string payStatus = item.value("payment_status", "pending");
                if (payStatus == "paid") {
                    ImGui::TextColored(ImVec4(0.25f, 0.75f, 0.45f, 1.0f), "Оплачено");
                } else if (payStatus == "upon_check_in") {
                    ImGui::TextColored(ImVec4(0.95f, 0.65f, 0.25f, 1.0f), "При заселенні");
                } else {
                    ImGui::TextColored(ImVec4(0.95f, 0.35f, 0.35f, 1.0f), "Очікує");
                }

                ImGui::TableSetColumnIndex(10);
                string status = item.value("status", "UNKNOWN");
                string statusText = "Невідомо";
                if (status == "RESERVED" || status == "reserved") statusText = "Зарезервовано";
                else if (status == "CONFIRMED" || status == "confirmed") statusText = "Активне";
                else if (status == "CANCELLED" || status == "cancelled") statusText = "Скасовано";
                ImGui::Text("%s", statusText.c_str());

                ImGui::TableSetColumnIndex(11);
                ImGui::PushID(bId);
                if (status != "CANCELLED" && status != "cancelled") {
                    if (ImGui::Button("Редагувати")) {
                        editBookingId = bId;
                        snprintf(editBufName, sizeof(editBufName), "%s", item.value("fullname", "").c_str());
                        snprintf(editBufPhone, sizeof(editBufPhone), "%s", item.value("phone", "").c_str());
                        snprintf(editBufPass, sizeof(editBufPass), "%s", item.value("passport", "").c_str());
                        snprintf(editBufIn, sizeof(editBufIn), "%s", item.value("checkin", "").c_str());
                        snprintf(editBufOut, sizeof(editBufOut), "%s", item.value("checkout", "").c_str());
                        openEditModalWindow = true;
                    }
                    ImGui::SameLine();
                    if ((status == "RESERVED" || status == "reserved") && (userRoleGlobalIdx == 1 || userRoleGlobalIdx == 3)) {
                        if (ImGui::Button("Підтвердити")) {
                            json req = { {"command", "CONFIRM_BOOKING"}, {"id", bId} };
                            thread(sendJsonRequest, req).detach();
                        }
                        ImGui::SameLine();
                    }
                    if (ImGui::Button("Скасувати")) {
                        bookingIdToCancel = bId;
                        openCancelModalWindow = true;
                    }
                } else {
                    ImGui::TextColored(ImVec4(0.95f, 0.35f, 0.35f, 1.0f), "Скасовано");
                }
                ImGui::PopID();
            }
        }
        ImGui::EndTable();
    }

    if (openEditModalWindow) { ImGui::OpenPopup("Картка редагування"); openEditModalWindow = false; }
    if (ImGui::BeginPopupModal("Картка редагування", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Редагування параметрів ордера №%d", editBookingId);
        ImGui::InputText("ПІБ клієнта", editBufName, sizeof(editBufName));
        ImGui::InputText("Моб. телефон", editBufPhone, sizeof(editBufPhone)); applyPhoneMask(editBufPhone);
        ImGui::InputText("Паспорт", editBufPass, sizeof(editBufPass));
        ImGui::InputText("Дата заселення", editBufIn, sizeof(editBufIn)); applyDateMask(editBufIn);
        ImGui::InputText("Дата виселення", editBufOut, sizeof(editBufOut)); applyDateMask(editBufOut);

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        if (ImGui::Button("Зберегти зміни", ImVec2(140, 0))) {
            json req = {
                {"command", "UPDATE_BOOKING_DATA"}, {"id", editBookingId},
                {"fullname", string(editBufName)}, {"phone", string(editBufPhone)}, {"passport", string(editBufPass)},
                {"checkin", string(editBufIn)}, {"checkout", string(editBufOut)}
            };
            thread(sendJsonRequest, req).detach();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Закрити", ImVec2(140, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    if (openCancelModalWindow) { ImGui::OpenPopup("Підтвердження скасування"); openCancelModalWindow = false; }
    if (ImGui::BeginPopupModal("Підтвердження скасування", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Ви дійсно хочете скасувати це бронювання?");
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        if (ImGui::Button("Так", ImVec2(120, 0))) {
            json req = { {"command", "CANCEL_BOOKING"}, {"id", bookingIdToCancel} };
            thread(sendJsonRequest, req).detach();
            bookingIdToCancel = -1;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Ні", ImVec2(120, 0))) { bookingIdToCancel = -1; ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}