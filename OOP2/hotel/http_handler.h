#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include "hotel.h"
#include <string>

// Перевірка кросплатформенності сокетів для CLion (Windows/Linux)
#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
#else
    #define SOCKET int
#endif

// Декларації функцій шлюзу сервера
std::string sanitizeHtml(const std::string& str);
int calculateServerDays(const std::string& checkIn, const std::string& checkOut);
void handleHttpGuest(SOCKET clientSocket, HotelSystem& hotel, const std::string& rawRequest);

#endif // HTTP_HANDLER_H