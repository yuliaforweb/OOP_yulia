#include <iostream>
#include <Windows.h>
#include "hotel.h"

using namespace std;

// Демонстрація поліморфізму через посилання
void showRoomDetails(const Room& r) {
    cout << "Опис: "
         << r.getDescription()
         << endl;
}

void printMainMenu() {
    cout << "\n╔══════════════════════════════╗\n";
    cout << "║  Iнформаційна система Готель ║\n";
    cout << "╠══════════════════════════════╣\n";
    cout << "║  1. Знайти вільні номери     ║\n";
    cout << "║  2. Створити бронювання      ║\n";
    cout << "║  3. Скасувати бронювання     ║\n";
    cout << "║  4. Оформити заїзд           ║\n";
    cout << "║  5. Оформити виїзд           ║\n";
    cout << "║  6. Список гостей            ║\n";
    cout << "║  7. Список бронювань         ║\n";
    cout << "║  8. Звіт по зайнятості       ║\n";
    cout << "║  0. Зберегти та вийти        ║\n";
    cout << "╚══════════════════════════════╝\n";
    cout << "Ваш вибiр: ";
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    HotelSystem hotel;
    hotel.loadData();

    hotel.addRoom(new StandardRoom(101,1, 800.0,2));
    hotel.addRoom(new StandardRoom(102,1, 800.0,2));
    hotel.addRoom(new StandardRoom(103,1, 900.0,3));
    hotel.addRoom(new DeluxeRoom (201,2,1500.0,2,true));
    hotel.addRoom(new DeluxeRoom (202,2,1500.0,3,false));
    hotel.addRoom(new Suite      (301,3,3500.0,4,true,2));
    hotel.addRoom(new Suite      (302,3,4200.0,6,true,3));

    bool running = true;
    while (running) {
        printMainMenu();
        int choice = 0;
        cin >> choice;
        switch (choice) {
            case 1: hotel.showAvailableRooms(); break;
            case 2: hotel.createBooking();      break;
            case 3: hotel.cancelBooking();      break;
            case 4: hotel.checkIn();            break;
            case 5: hotel.checkOut();           break;
            case 6: hotel.showGuests();         break;
            case 7: hotel.showBookings();       break;
            case 8: hotel.generateReport();     break;
            case 0:
                hotel.saveData();
                running = false;
                break;
            default:
                cout << "Невiрний вибiр." << endl;
        }
    }
    return 0;
}
