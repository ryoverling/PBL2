#include "user.h"

User::User() {
    username = "";
    password = "";
    name = "";
    loggedIn = false;
}

User::User(string username, string password, string name) {
    this->username = username;
    this->password = password;
    this->name = name;
    loggedIn = false;
}

string User::getUsername() const { return username; }
string User::getPassword() const { return password; }
string User::getName() const { return name; }

bool User::login(string username, string password) {
    loggedIn = (this->username == username && this->password == password);
    return loggedIn;
}

void User::logout() {
    loggedIn = false;
}

bool User::isLoggedIn() const {
    return loggedIn;
}
