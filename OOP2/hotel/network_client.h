#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <string>

// функція для відправки JSON запитів на готельний сервер
std::string sendPostRequest(const std::string& jsonPayload);

#endif // NETWORK_CLIENT_H