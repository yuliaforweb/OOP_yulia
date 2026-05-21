#include "auth_manager.h"
#include <libpq-fe.h>
#include <iostream>

using namespace std;

AuthManager::AuthManager() {
    // конструктор модуля перевірки доступу працівників
}

string AuthManager::checkCredentials(int roleIdx, const string& password) {
    string detectedRole = "";

    const char* conninfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Збій підключення модуля авторизації до БД: " << PQerrorMessage(conn) << endl;
        PQfinish(conn);
        return detectedRole;
    }

    // переписав імена стовпців на roleid та passwordhash без підкреслень
    string query = "SELECT rolename FROM staff_users WHERE roleid = " + to_string(roleIdx) +
                   " AND passwordhash = '" + password + "';";

    PGresult* res = PQexec(conn, query.c_str());

    // обов'язкова перевірка на коректність виконання самого SQL запиту
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        cerr << "Помилка виконання SQL запиту авторизації персоналу: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        PQfinish(conn);
        return detectedRole;
    }

    if (PQntuples(res) > 0) {
        detectedRole = PQgetvalue(res, 0, 0);
    }

    PQclear(res);
    PQfinish(conn);
    return detectedRole;
}

bool AuthManager::verifyLogin(int roleIdx, const string& password) {
    return !checkCredentials(roleIdx, password).empty();
}

bool AuthManager::verifyLogin(const string& roleText, const string& password) {
    int roleIdx = 0;
    if (roleText == "Адміністратор") roleIdx = 1;
    else if (roleText == "Менеджер") roleIdx = 2;
    else if (roleText == "Рецепція") roleIdx = 3;

    return verifyLogin(roleIdx, password);
}