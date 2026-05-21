#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>

class DatabaseManager {
public:
    DatabaseManager();

    // метод для автоматичного створення та валідації таблиць готелю
    bool initializeDatabase();
};

#endif // DATABASE_MANAGER_H