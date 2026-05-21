#include "database_manager.h"
#include <libpq-fe.h>
#include <iostream>

using namespace std;

DatabaseManager::DatabaseManager() {
    // конструктор менеджера бази даних готелю
}

bool DatabaseManager::initializeDatabase() {
    const char* conninfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Помилка ініціалізації бази даних готелю: " << PQerrorMessage(conn) << endl;
        PQfinish(conn);
        return false;
    }

    string createRoomsTable =
        "CREATE TABLE IF NOT EXISTS rooms ("
        "number INTEGER PRIMARY KEY, "
        "description TEXT NOT NULL, "
        "price REAL NOT NULL"
        ");";

    string createGuestsTable =
        "CREATE TABLE IF NOT EXISTS guests ("
        "id SERIAL PRIMARY KEY, "
        "fullname TEXT NOT NULL, "
        "phone TEXT, "
        "passport TEXT NOT NULL UNIQUE, "
        "email TEXT"
        ");";

    string createBookingsTable =
        "CREATE TABLE IF NOT EXISTS bookings ("
        "id SERIAL PRIMARY KEY, "
        "guestid INTEGER REFERENCES guests(id), "
        "roomnumber INTEGER REFERENCES rooms(number), "
        "checkin TEXT, "
        "checkout TEXT, "
        "totalcost REAL, "
        "status TEXT"
        ");";

    string createStaffTable =
        "CREATE TABLE IF NOT EXISTS staff_users ("
        "roleid INTEGER PRIMARY KEY, "
        "passwordhash TEXT NOT NULL, "
        "rolename TEXT NOT NULL"
        ");";

    PGresult* res = PQexec(conn, createRoomsTable.c_str());
    PQclear(res);

    res = PQexec(conn, createGuestsTable.c_str());
    PQclear(res);

    res = PQexec(conn, createBookingsTable.c_str());
    PQclear(res);

    res = PQexec(conn, createStaffTable.c_str());
    PQclear(res);

    string checkRooms = "SELECT COUNT(*) FROM rooms;";
    res = PQexec(conn, checkRooms.c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK && atoi(PQgetvalue(res, 0, 0)) == 0) {
        PQclear(res);
        string insertRooms =
            "INSERT INTO rooms (number, description, price) VALUES "
            "(101, 'Стандартний 2-місний (1 поверх)', 800.0), "
            "(102, 'Стандартний 2-місний (1 поверх)', 800.0), "
            "(103, 'Стандартний 3-місний (1 поверх)', 900.0), "
            "(201, 'Делюкс 2-місний з балконом (2 поверх)', 1500.0), "
            "(202, 'Делюкс 3-місний без балкона (2 поверх)', 1500.0), "
            "(301, 'Люкс 4-місний (3 поверх, джакузі, 2 кімнати)', 3500.0), "
            "(302, 'Люкс 6-місний (3 поверх, джакузі, 3 кімнати)', 4200.0);";
        res = PQexec(conn, insertRooms.c_str());
    }
    PQclear(res);

    string checkStaff = "SELECT COUNT(*) FROM staff_users;";
    res = PQexec(conn, checkStaff.c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK && atoi(PQgetvalue(res, 0, 0)) == 0) {
        PQclear(res);
        string insertStaff =
            "INSERT INTO staff_users (roleid, passwordhash, rolename) VALUES "
            "(1, 'admin123', 'Адміністратор'), "
            "(2, 'manager123', 'Менеджер'), "
            "(3, 'reception123', 'Рецепція');";
        res = PQexec(conn, insertStaff.c_str());
    }
    PQclear(res);

    PQfinish(conn);
    return true;
}