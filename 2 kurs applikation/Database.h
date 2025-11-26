#pragma once
#include <string>
#include <vector>
#include "User.h"
#include "Apartment.h"

class Database {
public:
    Database();
    ~Database();

    bool open(const std::string& filename);
    void close();

    bool registerUser(const std::string& username, const std::string& password);
    User* loginUser(const std::string& username, const std::string& password);
    bool updateUserBalance(int userId, double newBalance);

    std::vector<Apartment> getAllApartments();
    std::vector<Apartment> getApartmentsByOwner(int ownerId);
    bool addApartment(const Apartment& apt);
    Apartment* getApartmentById(int id);

    bool buyApartment(int apartmentId, int buyerId, double price);
    std::vector<std::vector<std::string>> getTransactionsByUser(int userId);

    bool addToFavorites(int userId, int apartmentId);
    bool removeFromFavorites(int userId, int apartmentId);
    std::vector<Apartment> getFavoriteApartments(int userId);

    std::vector<int> getUserApartmentIds(int userId);

private:
    void createTables();
    void* db = nullptr;
};