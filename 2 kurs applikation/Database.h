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
    std::vector<int> getUserApartmentIds(int userId);


    std::vector<Apartment> getAllApartments();
    std::vector<Apartment> getApartmentsByOwner(int ownerId);
    bool addApartment(const Apartment& apt);
    bool buyApartment(int apartmentId, int newOwnerId, double price);
    Apartment* getApartmentById(int id);

private:
    void createTables();

    void* db = nullptr; // void* to avoid including sqlite3.h in header
};