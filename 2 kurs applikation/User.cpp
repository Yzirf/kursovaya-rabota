#include "User.h"

User::User() : id(-1), balance(0.0) {}

User::User(int id, const std::string& username, const std::string& password, double balance)
    : id(id), username(username), password(password), balance(balance) {
}

int User::getId() const { return id; }
std::string User::getUsername() const { return username; }
std::string User::getPassword() const { return password; }
double User::getBalance() const { return balance; }
const std::vector<int>& User::getOwnedApartmentIds() const { return ownedApartmentIds; }

void User::setId(int id) { this->id = id; }
void User::setUsername(const std::string& username) { this->username = username; }
void User::setPassword(const std::string& password) { this->password = password; }
void User::setBalance(double balance) { this->balance = balance; }
void User::setOwnedApartmentIds(const std::vector<int>& ids) { ownedApartmentIds = ids; }
void User::addOwnedApartmentId(int aptId) { ownedApartmentIds.push_back(aptId); }
void User::removeOwnedApartmentId(int aptId) {
    ownedApartmentIds.erase(
        std::remove(ownedApartmentIds.begin(), ownedApartmentIds.end(), aptId),
        ownedApartmentIds.end()
    );
}