#include "report_manager.h"
#include <iostream>
#include <iomanip>

using namespace std;

ReportManager::ReportManager(const vector<Room*>& r, const vector<Booking*>& b) : rooms(r), bookings(b) {}

void ReportManager::generateOccupancyReport() const {
    int total = rooms.size();
    int freeCount = 0;
    for (auto* r : rooms) {
        if (r->getStatus() == RoomStatus::FREE) freeCount++;
    }
    cout << "\n=== СТАН НОМЕРНОГО ФОНДУ ===\n";
    cout << left << setw(30) << "Загально номерiв:" << total << endl;
    cout << left << setw(30) << "Вiльних номерiв:" << freeCount << endl;
}

void ReportManager::generateFinancialReport() const {
    double total = 0;
    for (auto* b : bookings) {
        if (b->getStatus() == BookingStatus::COMPLETED) {
            total += b->getTotalCost();
        }
    }
    cout << "\n=== ЗАГАЛЬНИЙ ПРИБУТОК ===\nСума: " << total << " грн" << endl;
}