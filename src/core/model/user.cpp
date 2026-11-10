#include "user.h"

User::User() = default;

User::User(std::string username, std::string password, std::string name)
    : username(username), password(password), name(name) {}

std::string User::getUsername() const { return username; }
std::string User::getPassword() const { return password; }
std::string User::getName() const { return name; }

void User::setPassword(const std::string& newPassword) { password = newPassword; }
