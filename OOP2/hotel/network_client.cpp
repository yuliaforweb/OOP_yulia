#include "network_client.h"
#include <iostream>
#include <sstream>

// перевіряю ОС для правильних системних сокетів
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>

    typedef int SOCKET;
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
    #define closesocket(s) close(s)
#endif

using namespace std;

string sendPostRequest(const string& jsonPayload) {
    // якщо запускаємо під віндовс, стартую віндові сокети
    #if defined(_WIN32)
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            return "{\"status\":\"ERROR\",\"message\":\"WSAStartup failed\"}";
        }
    #endif

    // створюю стандартний потоковий сокет
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        #if defined(_WIN32)
            WSACleanup();
        #endif
        return "{\"status\":\"ERROR\",\"message\":\"Socket creation failed\"}";
    }

    // налаштовую адресу структури на локалхост та порт 8888
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);

    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        closesocket(sock);
        #if defined(_WIN32)
            WSACleanup();
        #endif
        return "{\"status\":\"ERROR\",\"message\":\"Invalid address\"}";
    }

    // намагаюся підключитися до запущеного сервера готелю
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        closesocket(sock);
        #if defined(_WIN32)
            WSACleanup();
        #endif
        return "{\"status\":\"ERROR\",\"message\":\"Connection to server failed\"}";
    }

    // збираю правильний HTTP пакет, щоб http_handler на сервері зміг його розпарсити
    stringstream ss;
    ss << "POST /api HTTP/1.1\r\n";
    ss << "Host: 127.0.0.1:8888\r\n";
    ss << "Content-Type: application/json; charset=utf-8\r\n";
    ss << "Content-Length: " << jsonPayload.length() << "\r\n";
    ss << "Connection: close\r\n\r\n";
    ss << jsonPayload;

    string httpRequest = ss.str();

    // відправляю зібраний HTTP пакет у мережу
    if (send(sock, httpRequest.c_str(), httpRequest.length(), 0) == SOCKET_ERROR) {
        closesocket(sock);
        #if defined(_WIN32)
            WSACleanup();
        #endif
        return "{\"status\":\"ERROR\",\"message\":\"Send failed\"}";
    }

    // читаю повну відповідь сервера у буфер
    string response;
    char buffer[4096];
    int bytesRead = 0;

    while ((bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';
        response.append(buffer, bytesRead);
    }

    // закриваю поточний сокет
    closesocket(sock);
    #if defined(_WIN32)
        WSACleanup();
    #endif

    // шукаю де закінчуються HTTP заголовки відповіді і починається чистий JSON
    size_t jsonStart = response.find("\r\n\r\n");
    if (jsonStart != string::npos) {
        return response.substr(jsonStart + 4);
    }

    return response;
}