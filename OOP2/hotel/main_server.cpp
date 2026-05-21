#include <iostream>
#include <winsock2.h>
#include <thread>
#include <string>
#include <windows.h> // підключаю для налаштування кодування консолі в CLion
#include "hotel.h"
#include "http_handler.h"
#include "db_initializer.h" // мій новий файл ініціалізатора бази даних
#include "logger.h"

using namespace std;

// Уніфікована функція обробки підключень
void processClientConnection(SOCKET clientSocket, HotelSystem& hotel) {
    char buffer[4096] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived > 0) {
        string rawRequest(buffer, bytesReceived);

        // Оскільки network_client.cpp надсилає HTTP POST, ми передаємо всі
        // запити в http_handler. Він сам розбереться, де запит сайту, а де ImGui.
        handleHttpGuest(clientSocket, hotel, rawRequest);
    } else {
        closesocket(clientSocket);
    }
}

int main() {
    // Адаптую консоль CLion під коректне виведення кирилиці
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    // Першим ділом запускаю автоматичну перевірку та створення бази даних
    initializeDatabase();

    HotelSystem hotel;
    hotel.loadData();

    // Ініціалізую бібліотеку сокетів Windows
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logger::error("Не вдалося ініціалізувати бібліотеку Winsock");
        return 1;
    }

    // Створюю головний прослуховуючий сокет
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        Logger::error("Помилка створення серверного сокета");
        WSACleanup();
        return 1;
    }

    // Налаштовую параметри мережевого підключення до порту 8888
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8888);

    // Прив'язую сокет до порту
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        Logger::error("Помилка зв'язку сокета з портом 8888");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Переводжу сервер у режим очікування з'єднань
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        Logger::error("Помилка переведення сокета в режим listen");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    Logger::info("Сервер готелю успішно запущено на порту 8888");

    // Головний нескінченний цикл для приймання нових підключень
    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket != INVALID_SOCKET) {
            // Запускаю обробку кожного клієнта в окремому потоці, щоб сервер не блокувався
            thread(processClientConnection, clientSocket, ref(hotel)).detach();
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}