#pragma once
#include <string>

struct Apartment {
    int id = -1;
    int ownerId = -1;
    std::string ownerUsername;
    double price = 0.0;
    double area = 0.0;
    int rooms = 0;
    bool isNewBuilding = false;
    int yearBuilt = 0;
    int floor = 0;
    std::string address;

    double pricePerSqMeter() const {
        return area > 0 ? price / area : 0.0;
    }
};