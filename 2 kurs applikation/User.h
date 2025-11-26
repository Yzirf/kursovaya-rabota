#pragma once
#include <string>
#include <vector>

class User {
private:
    int id;
    std::string username;
    std::string password;
    double balance;

public:
    User();
    User(int id, const std::string& username, const std::string& password, double balance = 0.0);

    int getId() const;
    std::string getUsername() const;
    std::string getPassword() const;
    double getBalance() const;

    void setId(int id);
    void setUsername(const std::string& username);
    void setPassword(const std::string& password);
    void setBalance(double balance);
};