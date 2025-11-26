#include "User.h"

User::User() : id(-1), balance(0.0) {}

User::User(int id, const std::string& username, const std::string& password, double balance)
    : id(id), username(username), password(password), balance(balance) {
}

int User::getId() const { return id; }
std::string User::getUsername() const { return username; }
std::string User::getPassword() const { return password; }
double User::getBalance() const { return balance; }

void User::setId(int id) { this->id = id; }
void User::setUsername(const std::string& username) { this->username = username; }
void User::setPassword(const std::string& password) { this->password = password; }
void User::setBalance(double balance) { this->balance = balance; }