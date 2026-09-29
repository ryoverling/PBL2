#pragma once

#include <string>
using namespace std;

class User {
private:
    string username;
    string password;
    string name;
    bool loggedIn;

public:
    User();
    User(string username, string password, string name);

    string getUsername() const;
    string getPassword() const;
    string getName() const;

    bool login(string username, string password);
    void logout();
    bool isLoggedIn() const;
};
