#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include <string>

class AuthManager {
public:
    AuthManager();

    std::string checkCredentials(int roleIdx, const std::string& password);

    bool verifyLogin(int roleIdx, const std::string& password);
    bool verifyLogin(const std::string& roleText, const std::string& password);
};

#endif // AUTH_MANAGER_H