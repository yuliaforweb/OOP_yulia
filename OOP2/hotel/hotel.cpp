#include "hotel.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std;

HotelSystem::~HotelSystem() {
    for (auto* b : bookings) delete b;
    for (auto* g : guests)   delete g;
    for (auto* r : rooms)    delete r;
}

Room* HotelSystem::operator[](int index) {
    if (index >= 0 && index < (int)rooms.size()) {
        return rooms[index];
    }
    return nullptr; 
}

void HotelSystem::addRoom(Room* r) {
    rooms.push_back(r);
}

void HotelSystem::split(const string& s, char del,
                        vector<string>& out) {
    stringstream ss(s);
    string token;
    while (getline(ss, token, del))
        out.push_back(token);
}

bool HotelSystem::isValidDate(const string& d) {
    if (d.size() != 10) return false;
    if (d[2]!='.' || d[5]!='.') return false;
    int day  = stoi(d.substr(0,2));
    int mon  = stoi(d.substr(3,2));
    int year = stoi(d.substr(6,4));
    return day>=1 && day<=31 &&
           mon>=1 && mon<=12 &&
           year>=2024;
}

vector<Room*> HotelSystem::checkAvailability(
        const string& ci, const string& co) {
    vector<Room*> result;
    for (auto* room : rooms) {
        bool available = true;
        for (auto* b : bookings) {
            if (b->getRoom() == room &&
                b->getStatus()==BookingStatus::ACTIVE &&
                b->overlaps(ci, co)) {
                available = false;
                break;
            }
        }
        if (available) result.push_back(room);
    }
    return result;
}

void HotelSystem::showAvailableRooms() {
    string ci, co;
    cout << "Дата заїзду (дд.мм.рррр): ";
    cin >> ci;
    cout << "Дата виїзду (дд.мм.рррр): ";
    cin >> co;

    if (!isValidDate(ci) || !isValidDate(co)) {
        cout << "Невірний формат дати." << endl;
        return;
    }

    if (Booking::dateToInt(co) <= Booking::dateToInt(ci)) {
        cout << "Помилка: Дата виїзду має бути пізніше дати заїзду! Оберіть, будь ласка, коректні дати заїзду та виїзду." << endl;
        return;
    }
    auto available = checkAvailability(ci, co);
    if (available.empty()) {
        cout << "На жаль, вільних номерів немає." << endl;
        return;
    }
    cout << "Вільні номери:" << endl;
    for (auto* r : available) {
        cout << "  #" << r->getNumber()
             << " | " << r->getDescription()
             << " | " << fixed
             << setprecision(2)
             << r->getPricePerNight()
             << " грн/доб" << endl;
    }
}

Guest* HotelSystem::findOrCreateGuest() {
    string ln, fn, mn, passport, ph, dob, em;
    cout << "Прiзвище: ";     cin >> ln;
    cout << "Iм'я: ";         cin >> fn;
    cout << "По батьковi: ";  cin >> mn;
    cout << "Серiя та номер паспорта (без пробілів): ";
    cin.ignore();
    getline(cin, passport);
    for (auto* g : guests) {
        if (g->getPassport() == passport) {
            cout << "Гостя знайдено в базі (ID #" << g->getId() << ")." << endl;
            return g;
        }
    }

    cout << "Телефон: ";      cin >> ph;
    cout << "Дата народження (у форматі дд.мм.рррр.): "; cin >> dob;
    cout << "Email: ";        cin >> em;

    auto* g = new Guest(nextGuestId++, fn, ln, mn, ph, passport, dob, em);
    guests.push_back(g);
    cout << "Нового гостя зареєстровано." << endl;
    return g;
}

void HotelSystem::createBooking() {
    string ci, co;
    cout << "Дата заїзду: ";  cin >> ci;
    cout << "Дата виїзду: ";  cin >> co;

    if (!isValidDate(ci) || !isValidDate(co)) {
        cout << "Помилка: Невірний формат дати! Повторіть, будь ласка, бронювання вказавши дати у форматі дд.мм.рррр" << endl;
        return;
    }

    if (Booking::dateToInt(co) <= Booking::dateToInt(ci)) {
        cout << "Помилка: Дата виїзду має бути пізніше дати заїзду" << endl;
        return;
    }

    auto avail = checkAvailability(ci, co);
    if (avail.empty()) {
        cout << "Немає вільних номерів." << endl;
        return;
    }
    for (int i=0;i<(int)avail.size();i++)
        cout << i+1 << ". #"
             << avail[i]->getNumber()
             << " " << avail[i]->getDescription()
             << endl;
    cout << "Оберiть номер (1-"
         << avail.size() << "): ";
    int ch; cin >> ch;
    if (ch<1||ch>(int)avail.size()) return;
    Room* room = avail[ch-1];
    Guest* guest = findOrCreateGuest();
    auto* b = new Booking(nextBookingId++,
                          guest, room, ci, co);
    b->calculateCost();
    bookings.push_back(b);
    room->setStatus(RoomStatus::BOOKED);
    cout << "\nБронювання успішно створено!" << endl;
    b->printInfo();

    saveData();
}

void HotelSystem::cancelBooking() {
    int id;
    cout << "ID бронювання: "; cin >> id;
    for (auto* b : bookings) {
        if (b->getId()==id &&
            b->getStatus()==BookingStatus::ACTIVE) {
            b->cancel();
            b->getRoom()->setStatus(RoomStatus::FREE);
            cout << "Скасовано." << endl;
            saveData();
            return;
        }
    }
    cout << "Бронювання не знайдено." << endl;
}

void HotelSystem::checkIn() {
    int id;
    cout << "ID бронювання: "; cin >> id;
    for (auto* b : bookings) {
        if (b->getId()==id &&
            b->getStatus()==BookingStatus::ACTIVE) {
            b->getRoom()->setStatus(RoomStatus::OCCUPIED);
            cout << "Заїзд оформлено." << endl;
            saveData(); return;
        }
    }
    cout << "Бронювання не знайдено." << endl;
}

void HotelSystem::checkOut() {
    int id;
    cout << "ID бронювання: "; cin >> id;
    for (auto* b : bookings) {
        if (b->getId()==id &&
            b->getStatus()==BookingStatus::ACTIVE) {
            b->complete();
            b->getRoom()->setStatus(RoomStatus::FREE);
            cout << "Виїзд оформлено. "
                 << "Дякуємо за перебування!" << endl;
            saveData(); return;
        }
    }
    cout << "Бронювання не знайдено." << endl;
}

void HotelSystem::showGuests() const {
    if (guests.empty()) {
        cout << "Гостей немає." << endl; return;
    }
    for (auto* g : guests)
        cout << g->getInfo() << endl;
}

void HotelSystem::showBookings() const {
    if (bookings.empty()) {
        cout << "Бронювань немає." << endl; return;
    }
    for (auto* b : bookings) b->printInfo();
}

void HotelSystem::generateReport() const {
    int free=0, booked=0, occupied=0;
    for (auto* r : rooms) {
        if (r->getStatus()==RoomStatus::FREE)
            free++;
        else if (r->getStatus()==RoomStatus::BOOKED)
            booked++;
        else occupied++;
    }
    cout << "=== Звіт про зайнятість ===" << endl;
    cout << "Всього номерів : " << rooms.size() << endl;
    cout << "Вільних         : " << free     << endl;
    cout << "Заброньованих   : " << booked   << endl;
    cout << "Зайнятих        : " << occupied << endl;
}

void HotelSystem::saveData() const {
    // Збереження гостей
    ofstream fg("guests.dat");
    for (auto* g : guests)
        fg << g->getId()       << "|"
           << g->getFullName() << "|"
           << g->getPhone()    << "|"
           << g->getPassport() << "|"
           << g->getEmail()    << "\n";
    // Збереження бронювань
    ofstream fb("bookings.dat");
    for (auto* b : bookings) {
        string st =
            b->getStatus()==BookingStatus::ACTIVE    ? "A" :
            b->getStatus()==BookingStatus::CANCELLED ? "C" : "X";
        fb << b->getId()              << "|"
           << b->getGuest()->getId()  << "|"
           << b->getRoom()->getNumber()<< "|"
           << b->getCheckIn()         << "|"
           << b->getCheckOut()        << "|"
           << b->getTotalCost()       << "|"
           << st << "\n";
    }
}

void HotelSystem::loadData() {
    // Завантаження гостей
    ifstream fg("guests.dat");
    string line;
    while (getline(fg, line)) {
        vector<string> f;
        split(line, '|', f);
        if (f.size() < 5) continue;
        int id = stoi(f[0]);
        // ПІБ зберігається як одне поле
        auto* g = new Guest(id,
            f[1],"","",f[2],f[3],"",f[4]);
        guests.push_back(g);
        if (id >= nextGuestId) nextGuestId = id+1;
    }
    // Завантаження бронювань
    ifstream fb("bookings.dat");
    while (getline(fb, line)) {
        vector<string> f;
        split(line, '|', f);
        if (f.size() < 7) continue;
        int bid = stoi(f[0]);
        int gid = stoi(f[1]);
        int rnum= stoi(f[2]);
        Guest* g = nullptr;
        for (auto* gg : guests)
            if (gg->getId()==gid) { g=gg; break; }
        Room* r = nullptr;
        for (auto* rr : rooms)
            if (rr->getNumber()==rnum) { r=rr; break; }
        if (!g || !r) continue;
        auto* b = new Booking(bid, g, r, f[3], f[4]);
        b->calculateCost();
        if (f[6]=="C") b->cancel();
        if (f[6]=="X") b->complete();
        bookings.push_back(b);
        if (bid >= nextBookingId) nextBookingId=bid+1;
    }
}