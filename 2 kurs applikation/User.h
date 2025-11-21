#pragma once
#include <string>
#include <vector>

struct User {
    int id = -1;
    std::string username;
    std::string password;
    double balance = 0.0;
    std::vector<int> ownedApartmentIds;
};