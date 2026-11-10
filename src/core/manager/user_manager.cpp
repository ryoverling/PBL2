#include "user_manager.h"
#include "../storage/file_storage.h"
#include "category_manager.h"
#include <filesystem>
#include <cctype>

namespace fs = std::filesystem;

static bool equalsIgnoreCase(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) return false;
    }
    return true;
}

UserManager::UserManager() = default;

bool UserManager::loadUsers(const std::string& filePath) {
    if (!fs::exists(filePath)) {
        clear();
        return true;
    }
    return FileStorage::loadUsers(filePath, users);
}

bool UserManager::saveUsers(const std::string& filePath) const {
    return FileStorage::saveUsers(filePath, users);
}

bool UserManager::isValidUsername(const std::string& username) {
    if (username.empty() || username.length() > 20) return false;
    for (char c : username) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (!std::isalnum(uc) && uc != '_') {
            return false;
        }
    }
    return true;
}

bool UserManager::isValidPassword(const std::string& password) {
    if (password.length() < 4) return false;
    for (char c : password) {
        if (c == '|' || c == '\n' || c == '\r') {
            return false;
        }
    }
    return true;
}

bool UserManager::registerUser(const std::string& username, const std::string& password, const std::string& name) {
    if (!isValidUsername(username) || !isValidPassword(password)) return false;
    if (hasUser(username)) return false;

    std::string displayName = CategoryManager::normalizeName(name);
    if (displayName.empty()) displayName = username;

    users.pushBack(User(username, password, displayName));
    return true;
}

bool UserManager::login(const std::string& username, const std::string& password) const {
    const User* u = findByUsername(username);
    if (!u) return false;
    return u->getPassword() == password;
}

bool UserManager::changePassword(const std::string& username, const std::string& oldPassword, const std::string& newPassword) {
    if (!isValidPassword(newPassword)) return false;
    User* u = findByUsername(username);
    if (!u || u->getPassword() != oldPassword) return false;
    u->setPassword(newPassword);
    return true;
}

User* UserManager::findByUsername(const std::string& username) {
    for (Node<User>* cur = users.getHead(); cur != nullptr; cur = cur->next) {
        if (equalsIgnoreCase(cur->data.getUsername(), username)) {
            return &(cur->data);
        }
    }
    return nullptr;
}

const User* UserManager::findByUsername(const std::string& username) const {
    for (const Node<User>* cur = users.getHead(); cur != nullptr; cur = cur->next) {
        if (equalsIgnoreCase(cur->data.getUsername(), username)) {
            return &(cur->data);
        }
    }
    return nullptr;
}

bool UserManager::hasUser(const std::string& username) const {
    return findByUsername(username) != nullptr;
}

size_t UserManager::getCount() const {
    return users.getSize();
}

void UserManager::clear() {
    users.clear();
}

LinkedList<User>& UserManager::getUsers() {
    return users;
}

const LinkedList<User>& UserManager::getUsers() const {
    return users;
}
