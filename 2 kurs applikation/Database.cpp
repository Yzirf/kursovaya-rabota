#include "Database.h"
#include "sqlite3.h"
#include <iostream>
#include <sstream>
#include <ctime>

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

    const char* sql_transactions =
        "CREATE TABLE IF NOT EXISTS transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "buyer_id INTEGER NOT NULL,"
        "seller_id INTEGER NOT NULL,"
        "apartment_id INTEGER NOT NULL,"
        "amount REAL NOT NULL,"
        "transaction_date TEXT NOT NULL,"
        "FOREIGN KEY(buyer_id) REFERENCES users(id),"
        "FOREIGN KEY(seller_id) REFERENCES users(id),"
        "FOREIGN KEY(apartment_id) REFERENCES apartments(id));";

    const char* sql_favorites =
        "CREATE TABLE IF NOT EXISTS favorites ("
        "user_id INTEGER NOT NULL,"
        "apartment_id INTEGER NOT NULL,"
        "PRIMARY KEY(user_id, apartment_id),"
        "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE,"
        "FOREIGN KEY(apartment_id) REFERENCES apartments(id) ON DELETE CASCADE);";

    const char* sql_filters =
        "CREATE TABLE IF NOT EXISTS filters ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "name TEXT NOT NULL,"
        "min_price REAL,"
        "max_price REAL,"
        "min_area REAL,"
        "max_area REAL,"
        "rooms_min INTEGER,"
        "rooms_max INTEGER,"
        "is_new_building INTEGER,"
        "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE);";

    char* errMsg = nullptr;
    sqlite3_exec((sqlite3*)db, sql_users, nullptr, nullptr, &errMsg);
    if (errMsg) { std::cerr << "Ошибка users: " << errMsg << "\n"; sqlite3_free(errMsg); }

    sqlite3_exec((sqlite3*)db, sql_apartments, nullptr, nullptr, &errMsg);
    if (errMsg) { std::cerr << "Ошибка apartments: " << errMsg << "\n"; sqlite3_free(errMsg); }

    sqlite3_exec((sqlite3*)db, sql_transactions, nullptr, nullptr, &errMsg);
    if (errMsg) { std::cerr << "Ошибка transactions: " << errMsg << "\n"; sqlite3_free(errMsg); }

    sqlite3_exec((sqlite3*)db, sql_favorites, nullptr, nullptr, &errMsg);
    if (errMsg) { std::cerr << "Ошибка favorites: " << errMsg << "\n"; sqlite3_free(errMsg); }

    sqlite3_exec((sqlite3*)db, sql_filters, nullptr, nullptr, &errMsg);
    if (errMsg) { std::cerr << "Ошибка filters: " << errMsg << "\n"; sqlite3_free(errMsg); }
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
            currentUser = User(id, username, password, balance);
            sqlite3_finalize(stmt);
            return &currentUser;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
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
        result.emplace_back(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            (const char*)sqlite3_column_text(stmt, 2),
            sqlite3_column_double(stmt, 3),
            sqlite3_column_double(stmt, 4),
            sqlite3_column_int(stmt, 5),
            sqlite3_column_int(stmt, 6) != 0,
            sqlite3_column_int(stmt, 7),
            sqlite3_column_int(stmt, 8),
            (const char*)sqlite3_column_text(stmt, 9)
        );
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
        result.emplace_back(
            sqlite3_column_int(stmt, 0),
            ownerId,
            "",
            sqlite3_column_double(stmt, 2),
            sqlite3_column_double(stmt, 3),
            sqlite3_column_int(stmt, 4),
            sqlite3_column_int(stmt, 5) != 0,
            sqlite3_column_int(stmt, 6),
            sqlite3_column_int(stmt, 7),
            (const char*)sqlite3_column_text(stmt, 8)
        );
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
    sqlite3_bind_int(stmt, 1, apt.getOwnerId());
    sqlite3_bind_double(stmt, 2, apt.getPrice());
    sqlite3_bind_double(stmt, 3, apt.getArea());
    sqlite3_bind_int(stmt, 4, apt.getRooms());
    sqlite3_bind_int(stmt, 5, apt.getIsNewBuilding() ? 1 : 0);
    sqlite3_bind_int(stmt, 6, apt.getYearBuilt());
    sqlite3_bind_int(stmt, 7, apt.getFloor());
    sqlite3_bind_text(stmt, 8, apt.getAddress().c_str(), -1, SQLITE_STATIC);
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
        apt = Apartment(
            id,
            sqlite3_column_int(stmt, 0),
            "",
            sqlite3_column_double(stmt, 1),
            sqlite3_column_double(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_int(stmt, 4) != 0,
            sqlite3_column_int(stmt, 5),
            sqlite3_column_int(stmt, 6),
            (const char*)sqlite3_column_text(stmt, 7)
        );
        sqlite3_finalize(stmt);
        return &apt;
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

bool Database::buyApartment(int apartmentId, int buyerId, double price) {
    const char* sql_get = "SELECT owner_id FROM apartments WHERE id = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2((sqlite3*)db, sql_get, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, apartmentId);
    int sellerId = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sellerId = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    if (sellerId == -1) return false;

    const char* sql_update_apartment = "UPDATE apartments SET owner_id = ? WHERE id = ?";
    rc = sqlite3_prepare_v2((sqlite3*)db, sql_update_apartment, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, buyerId);
    sqlite3_bind_int(stmt, 2, apartmentId);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return false;

    const char* sql_update_seller = "UPDATE users SET balance = balance + ? WHERE id = ?";
    rc = sqlite3_prepare_v2((sqlite3*)db, sql_update_seller, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    sqlite3_bind_double(stmt, 1, price);
    sqlite3_bind_int(stmt, 2, sellerId);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return false;

    const char* sql_update_buyer = "UPDATE users SET balance = balance - ? WHERE id = ?";
    rc = sqlite3_prepare_v2((sqlite3*)db, sql_update_buyer, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    sqlite3_bind_double(stmt, 1, price);
    sqlite3_bind_int(stmt, 2, buyerId);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return false;

    time_t now = time(0);
    char buf[20];
    //strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    const char* sql_insert_tx =
        "INSERT INTO transactions (buyer_id, seller_id, apartment_id, amount, transaction_date) "
        "VALUES (?, ?, ?, ?, ?);";
    rc = sqlite3_prepare_v2((sqlite3*)db, sql_insert_tx, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, buyerId);
    sqlite3_bind_int(stmt, 2, sellerId);
    sqlite3_bind_int(stmt, 3, apartmentId);
    sqlite3_bind_double(stmt, 4, price);
    sqlite3_bind_text(stmt, 5, buf, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<std::vector<std::string>> Database::getTransactionsByUser(int userId) {
    std::vector<std::vector<std::string>> result;
    const char* sql =
        "SELECT t.amount, t.transaction_date, a.address, u1.username AS seller "
        "FROM transactions t "
        "JOIN apartments a ON t.apartment_id = a.id "
        "JOIN users u1 ON t.seller_id = u1.id "
        "WHERE t.buyer_id = ? OR t.seller_id = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2((sqlite3*)db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return result;
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, userId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::vector<std::string> row;
        row.push_back(std::to_string(sqlite3_column_double(stmt, 0)));
        row.push_back((const char*)sqlite3_column_text(stmt, 1));
        row.push_back((const char*)sqlite3_column_text(stmt, 2));
        row.push_back((const char*)sqlite3_column_text(stmt, 3));
        result.push_back(row);
    }
    sqlite3_finalize(stmt);
    return result;
}

bool Database::addToFavorites(int userId, int apartmentId) {
    const char* sql = "INSERT INTO favorites (user_id, apartment_id) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2((sqlite3*)db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, apartmentId);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::removeFromFavorites(int userId, int apartmentId) {
    const char* sql = "DELETE FROM favorites WHERE user_id = ? AND apartment_id = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2((sqlite3*)db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, apartmentId);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<Apartment> Database::getFavoriteApartments(int userId) {
    std::vector<Apartment> result;
    const char* sql =
        "SELECT a.id, a.owner_id, u.username, a.price, a.area, a.rooms, "
        "a.is_new_building, a.year_built, a.floor, a.address "
        "FROM favorites f "
        "JOIN apartments a ON f.apartment_id = a.id "
        "JOIN users u ON a.owner_id = u.id "
        "WHERE f.user_id = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2((sqlite3*)db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return result;
    sqlite3_bind_int(stmt, 1, userId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result.emplace_back(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            (const char*)sqlite3_column_text(stmt, 2),
            sqlite3_column_double(stmt, 3),
            sqlite3_column_double(stmt, 4),
            sqlite3_column_int(stmt, 5),
            sqlite3_column_int(stmt, 6) != 0,
            sqlite3_column_int(stmt, 7),
            sqlite3_column_int(stmt, 8),
            (const char*)sqlite3_column_text(stmt, 9)
        );
    }
    sqlite3_finalize(stmt);
    return result;
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