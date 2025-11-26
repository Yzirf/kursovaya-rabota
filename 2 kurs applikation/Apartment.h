#pragma once
#include <string>

class Apartment {
private:
    int id;
    int ownerId;
    std::string ownerUsername;
    double price;
    double area;
    int rooms;
    bool isNewBuilding;
    int yearBuilt;
    int floor;
    std::string address;

public:
    Apartment();
    Apartment(int id, int ownerId, const std::string& ownerUsername,
        double price, double area, int rooms, bool isNewBuilding,
        int yearBuilt, int floor, const std::string& address);

    int getId() const;
    int getOwnerId() const;
    std::string getOwnerUsername() const;
    double getPrice() const;
    double getArea() const;
    int getRooms() const;
    bool getIsNewBuilding() const;
    int getYearBuilt() const;
    int getFloor() const;
    std::string getAddress() const;
    double getPricePerSqMeter() const;

    void setId(int id);
    void setOwnerId(int ownerId);
    void setOwnerUsername(const std::string& ownerUsername);
    void setPrice(double price);
    void setArea(double area);
    void setRooms(int rooms);
    void setIsNewBuilding(bool isNew);
    void setYearBuilt(int year);
    void setFloor(int floor);
    void setAddress(const std::string& address);
};