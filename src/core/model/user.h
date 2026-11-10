#pragma once

#include <string>

// Thực thể Tài khoản người dùng
class User {
private:
    std::string username;
    std::string password;
    std::string name;

public:
    User();
    User(std::string username, std::string password, std::string name);

    std::string getUsername() const;
    std::string getPassword() const;
    std::string getName() const;

    void setPassword(const std::string& newPassword);
};
