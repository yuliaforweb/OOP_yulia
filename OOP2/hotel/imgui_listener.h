#ifndef HOTEL_IMGUI_LISTENER_H
#define HOTEL_IMGUI_LISTENER_H

#include <winsock2.h>
#include <string>
#include "hotel.h"

void handleImGuiClient(SOCKET clientSocket, HotelSystem& hotel, const std::string& firstPacket);

#endif