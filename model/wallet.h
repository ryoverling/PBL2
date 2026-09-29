#pragma once

#include <string>
using namespace std;

enum class WalletType {
    CASH,
    BANK,
    EWALLET,
    OTHER
};

string walletTypeToString(WalletType type);
WalletType walletTypeFromString(const string& value);

class Wallet {
private:
    int id;
    string name;
    WalletType type;
    double balance;

public:
    Wallet();
    Wallet(int id, string name, WalletType type, double balance);

    int getId() const;
    string getName() const;
    WalletType getType() const;
    double getBalance() const;

    void setName(string name);
    void setType(WalletType type);

    bool deposit(double amount);
    bool withdraw(double amount);
};
