#include "db_initializer.h"
#include "logger.h"
#include <libpq-fe.h>
#include <string>
#include <iostream>

using namespace std;

void initializeDatabase() {
    // підключення до системної бази postgres для перевірки наявності бази готелю
    const char* connPostgresInfo = "host=127.0.0.1 port=5432 dbname=postgres user=postgres password=root";
    PGconn* connPostgres = PQconnectdb(connPostgresInfo);
    if (PQstatus(connPostgres) != CONNECTION_OK) {
        Logger::error("Не вдалося підключитися до системної бази postgres для перевірки запуску");
        PQfinish(connPostgres);
        return;
    }

    string checkDbSql = "SELECT 1 FROM pg_database WHERE datname = 'hotel_db';";
    PGresult* resCheck = PQexec(connPostgres, checkDbSql.c_str());

    bool dbExists = false;
    if (PQresultStatus(resCheck) == PGRES_TUPLES_OK && PQntuples(resCheck) > 0) {
        dbExists = true;
    }
    PQclear(resCheck);

    if (dbExists) {
        PQfinish(connPostgres);
        return;
    }

    Logger::info("Виявлено перший запуск програми, починаю створення бази даних готелю");
    PGresult* resCreate = PQexec(connPostgres, "CREATE DATABASE hotel_db;");
    PQclear(resCreate);
    PQfinish(connPostgres);

    // підключення до вже створеної бази готелю, створюючи змінну conn
    const char* conninfo = "host=127.0.0.1 port=5432 dbname=hotel_db user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        Logger::error("Збій підключення до новоствореної бази даних");
        PQfinish(conn);
        return;
    }

    // структура таблиць через стандартні запити
    PQclear(PQexec(conn, "CREATE TABLE guests (id SERIAL PRIMARY KEY, fullname VARCHAR(255), phone VARCHAR(50), passport VARCHAR(50));"));
    PQclear(PQexec(conn, "CREATE TABLE rooms (number INT PRIMARY KEY, description VARCHAR(255), price NUMERIC(10, 2));"));
    PQclear(PQexec(conn, "CREATE TABLE bookings (id SERIAL PRIMARY KEY, guestid INT REFERENCES guests(id), "
                         "roomnumber INT REFERENCES rooms(number), checkin VARCHAR(50), checkout VARCHAR(50), totalcost NUMERIC(10, 2), status VARCHAR(50), "
                         "payment_method VARCHAR(50) DEFAULT 'pending', payment_status VARCHAR(50) DEFAULT 'upon_check_in');"));
    // додавання таблиці користувачів із колонкою rolename
    PQclear(PQexec(conn, "CREATE TABLE staff_users (roleid INT PRIMARY KEY, passwordhash VARCHAR(255), rolename VARCHAR(255));"));

    Logger::info("Заповнюю нову базу початковими номерами готелю та акаунтами ролей");
    PQclear(PQexec(conn, "INSERT INTO rooms (number, description, price) VALUES "
                         "(101, 'Стандартний 2-місний (1 поверх)', 800.00), "
                         "(102, 'Стандартний 2-місний (1 поверх)', 800.00), "
                         "(103, 'Стандартний 3-місний (1 поверх)', 900.00), "
                         "(201, 'Делюкс 2-місний з балконом (2 поверх)', 1500.00), "
                         "(202, 'Делюкс 3-місний без балкона (2 поверх)', 1500.00), "
                         "(301, 'Люкс 4-місний (3 поверх, джакузі, 2 кімнати)', 3500.00), "
                         "(302, 'Люкс 6-місний (3 поверх, джакузі, 3 кімнати)', 4200.00);"));


    PQclear(PQexec(conn, "INSERT INTO staff_users (roleid, passwordhash, rolename) VALUES (1, 'admin123', 'Адміністратор'), (2, 'manager123', 'Менеджер'), (3, 'reception123', 'Рецепція');"));

    PQfinish(conn);
    Logger::info("Автоматичне розгортання структури бази для першого запуску завершено");
}