#include "Apartment.h"
#include <algorithm>

Apartment::Apartment() : id(-1), ownerId(-1), price(0.0), area(0.0), rooms(0),
isNewBuilding(false), yearBuilt(0), floor(0) {
}

Apartment::Apartment(int id, int ownerId, const std::string& ownerUsername,
    double price, double area, int rooms, bool isNewBuilding,
    int yearBuilt, int floor, const std::string& address)
    : id(id), ownerId(ownerId), ownerUsername(ownerUsername), price(price), area(area),
    rooms(rooms), isNewBuilding(isNewBuilding), yearBuilt(yearBuilt), floor(floor), address(address) {
}

int Apartment::getId() const { return id; }
int Apartment::getOwnerId() const { return ownerId; }
std::string Apartment::getOwnerUsername() const { return ownerUsername; }
double Apartment::getPrice() const { return price; }
double Apartment::getArea() const { return area; }
int Apartment::getRooms() const { return rooms; }
bool Apartment::getIsNewBuilding() const { return isNewBuilding; }
int Apartment::getYearBuilt() const { return yearBuilt; }
int Apartment::getFloor() const { return floor; }
std::string Apartment::getAddress() const { return address; }
double Apartment::getPricePerSqMeter() const {
    return area > 0 ? price / area : 0.0;
}

void Apartment::setId(int id) { this->id = id; }
void Apartment::setOwnerId(int ownerId) { this->ownerId = ownerId; }
void Apartment::setOwnerUsername(const std::string& ownerUsername) { this->ownerUsername = ownerUsername; }
void Apartment::setPrice(double price) { this->price = price; }
void Apartment::setArea(double area) { this->area = area; }
void Apartment::setRooms(int rooms) { this->rooms = rooms; }
void Apartment::setIsNewBuilding(bool isNew) { this->isNewBuilding = isNew; }
void Apartment::setYearBuilt(int year) { this->yearBuilt = year; }
void Apartment::setFloor(int floor) { this->floor = floor; }
void Apartment::setAddress(const std::string& address) { this->address = address; }