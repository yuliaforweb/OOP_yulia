#include "guest.h"

string Guest::getInfo() const {
    return "Гість #" + to_string(guestId) + ": " +
           getFullName() +
           ", тел.: " + phone +
           ", паспорт: " + passportNumber +
           ", email: " + email;
}