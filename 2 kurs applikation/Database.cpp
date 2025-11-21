#include "Database.h"
#include "sqlite3.h"
#include <iostream>
#include <sstream>

Database::Database() = default;

Database::~Database() {
    close();
}

bool Database::open(const std::string& filename) {
    int rc = sqlite3_open(filename.c_str(), (sqlite3**)&db);
    if (rc != SQLITE_OK) {
        std::cerr << "Не удалось открыть БД: " << sqlite3_errmsg((sqlite3*)db) << "\n";
        return false;
    }
    createTables();
    return true;
}

void Database::close() {
    if (db) {
        sqlite3_close((sqlite3*)db);
        db = nullptr;
    }
}

void Database::createTables() {
    const char* sql_users =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password TEXT NOT NULL,"
        "balance REAL DEFAULT 0);";

    const char* sql_apartments =
        "CREATE TABLE IF NOT EXISTS apartments ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "owner_id INTEGER NOT NULL,"
        "price REAL NOT NULL,"
        "area REAL NOT NULL,"
        "rooms INTEGER NOT NULL,"
        "is_new_building INTEGER NOT NULL,"
        "year_built INTEGER NOT NULL,"
        "floor INTEGER NOT NULL,"
        "address TEXT NOT NULL,"
        "FOREIGN KEY(owner_id) REFERENCES users(id));";

    char* errMsg = nullptr;
    sqlite3_exec((sqlite3*)db, sql_users, nullptr, nullptr, &errMsg);
    if (errMsg) {
        std::cerr << "Ошибка создания таблицы users: " << errMsg << "\n";
        sqlite3_free(errMsg);
    }

    sqlite3_exec((sqlite3*)db, sql_apartments, nullptr, nullptr, &errMsg);
    if (errMsg) {
        std::cerr << "Ошибка создания таблицы apartments: " << errMsg << "\n";
        sqlite3_free(errMsg);
    }
}

bool Database::registerUser(const std::string& username, const std::string& password) {
    const char* sql = "INSERT INTO users (username, password, balance) VALUES (?, ?, 0.0);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2((sqlite3*)db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

User* loginUserCallback(User* user, const std::string& username, double balance, int id) {
    user->id = id;
    user->username = username;
    user->balance = balance;
    return user;
}

User* Database::loginUser(const std::string& username, const std::string& password) {
    const char* sql = "SELECT id, password, balance FROM users WHERE username = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2((sqlite3*)db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return nullptr;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    static User currentUser;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* storedPass = (const char*)sqlite3_column_text(stmt, 1);
        if (password == storedPass) {
            int id = sqlite3_column_int(stmt, 0);
            double balance = sqlite3_column_double(stmt, 2);
            currentUser.id = id;
            currentUser.username = username;
            currentUser.password = password;
            currentUser.balance = balance;
            currentUser.ownedApartmentIds = getUserApartmentIds(id);
            sqlite3_finalize(stmt);
            return &currentUser;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<int> Database::getUserApartmentIds(int userId) {
    std::vector<int> ids;
    const char* sql = "SELECT id FROM apartments WHERE owner_id = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2((sqlite3*)db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return ids;

    sqlite3_bind_int(stmt, 1, userId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ids.push_back(sqlite3_column_int(stmt, 0));
    }
    sqlite3_finalize(stmt);
    return ids;
}

bool Database::updateUserBalance(int userId, double newBalance) {
    const char* sql = "UPDATE users SET balance = ? WHERE id = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2((sqlite3*)db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_double(stmt, 1, newBalance);
    sqlite3_bind_int(stmt, 2, userId);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<Apartment> Database::getAllApartments() {
    std::vector<Apartment> result;
    const char* sql =
        "SELECT a.id, a.owner_id, u.username, a.price, a.area, a.rooms, "
        "a.is_new_building, a.year_built, a.floor, a.address "
        "FROM apartments a JOIN users u ON a.owner_id = u.id;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2((sqlite3*)db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return result;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Apartment a;
        a.id = sqlite3_column_int(stmt, 0);
        a.ownerId = sqlite3_column_int(stmt, 1);
        a.ownerUsername = (const char*)sqlite3_column_text(stmt, 2);
        a.price = sqlite3_column_double(stmt, 3);
        a.area = sqlite3_column_double(stmt, 4);
        a.rooms = sqlite3_column_int(stmt, 5);
        a.isNewBuilding = sqlite3_column_int(stmt, 6) != 0;
        a.yearBuilt = sqlite3_column_int(stmt, 7);
        a.floor = sqlite3_column_int(stmt, 8);
        a.address = (const char*)sqlite3_column_text(stmt, 9);
        result.push_back(a);
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<Apartment> Database::getApartmentsByOwner(int ownerId) {
    std::vector<Apartment> result;
    const char* sql =
        "SELECT id, owner_id, price, area, rooms, is_new_building, year_built, floor, address "
        "FROM apartments WHERE owner_id = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2((sqlite3*)db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return result;

    sqlite3_bind_int(stmt, 1, ownerId);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Apartment a;
        a.id = sqlite3_column_int(stmt, 0);
        a.ownerId = sqlite3_column_int(stmt, 1);
        a.price = sqlite3_column_double(stmt, 2);
        a.area = sqlite3_column_double(stmt, 3);
        a.rooms = sqlite3_column_int(stmt, 4);
        a.isNewBuilding = sqlite3_column_int(stmt, 5) != 0;
        a.yearBuilt = sqlite3_column_int(stmt, 6);
        a.floor = sqlite3_column_int(stmt, 7);
        a.address = (const char*)sqlite3_column_text(stmt, 8);
        result.push_back(a);
    }
    sqlite3_finalize(stmt);
    return result;
}

bool Database::addApartment(const Apartment& apt) {
    const char* sql =
        "INSERT INTO apartments (owner_id, price, area, rooms, is_new_building, year_built, floor, address) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2((sqlite3*)db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, apt.ownerId);
    sqlite3_bind_double(stmt, 2, apt.price);
    sqlite3_bind_double(stmt, 3, apt.area);
    sqlite3_bind_int(stmt, 4, apt.rooms);
    sqlite3_bind_int(stmt, 5, apt.isNewBuilding ? 1 : 0);
    sqlite3_bind_int(stmt, 6, apt.yearBuilt);
    sqlite3_bind_int(stmt, 7, apt.floor);
    sqlite3_bind_text(stmt, 8, apt.address.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::buyApartment(int apartmentId, int newOwnerId, double price) {
  
    const char* sql_get_owner = "SELECT owner_id FROM apartments WHERE id = ?";
    sqlite3_stmt* stmt;
    int oldOwnerId = -1;

    int rc = sqlite3_prepare_v2((sqlite3*)db, sql_get_owner, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, apartmentId);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        oldOwnerId = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    if (oldOwnerId == -1) return false;

    const char* sql_update = "UPDATE apartments SET owner_id = ? WHERE id = ?";
    rc = sqlite3_prepare_v2((sqlite3*)db, sql_update, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, newOwnerId);
    sqlite3_bind_int(stmt, 2, apartmentId);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return false;

    const char* sql_balance = "UPDATE users SET balance = balance + ? WHERE id = ?";
    rc = sqlite3_prepare_v2((sqlite3*)db, sql_balance, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    sqlite3_bind_double(stmt, 1, price);
    sqlite3_bind_int(stmt, 2, oldOwnerId);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

Apartment* Database::getApartmentById(int id) {
    const char* sql =
        "SELECT owner_id, price, area, rooms, is_new_building, year_built, floor, address "
        "FROM apartments WHERE id = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2((sqlite3*)db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return nullptr;

    sqlite3_bind_int(stmt, 1, id);
    static Apartment apt;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        apt.id = id;
        apt.ownerId = sqlite3_column_int(stmt, 0);
        apt.price = sqlite3_column_double(stmt, 1);
        apt.area = sqlite3_column_double(stmt, 2);
        apt.rooms = sqlite3_column_int(stmt, 3);
        apt.isNewBuilding = sqlite3_column_int(stmt, 4) != 0;
        apt.yearBuilt = sqlite3_column_int(stmt, 5);
        apt.floor = sqlite3_column_int(stmt, 6);
        apt.address = (const char*)sqlite3_column_text(stmt, 7);
        sqlite3_finalize(stmt);
        return &apt;
    }
    sqlite3_finalize(stmt);
    return nullptr;
}