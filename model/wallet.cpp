#include "wallet.h"
#include <cmath>

string walletTypeToString(WalletType type) {
    switch (type) {
        case WalletType::CASH: return "CASH";
        case WalletType::BANK: return "BANK";
        case WalletType::EWALLET: return "EWALLET";
        default: return "OTHER";
    }
}

WalletType walletTypeFromString(const string& value) {
    if (value == "CASH") return WalletType::CASH;
    if (value == "BANK") return WalletType::BANK;
    if (value == "EWALLET") return WalletType::EWALLET;
    return WalletType::OTHER;
}

Wallet::Wallet() {
    id = 0;
    name = "";
    type = WalletType::OTHER;
    balance = 0;
}

Wallet::Wallet(int id, string name, WalletType type, double balance) {
    this->id = id;
    this->name = name;
    this->type = type;
    this->balance = balance;
}

int Wallet::getId() const {
    return id;
}

string Wallet::getName() const {
    return name;
}

WalletType Wallet::getType() const {
    return type;
}

double Wallet::getBalance() const {
    return balance;
}

void Wallet::setName(string name) {
    this->name = name;
}

void Wallet::setType(WalletType type) {
    this->type = type;
}

bool Wallet::deposit(double amount) {
    if (!std::isfinite(amount) || amount <= 0 || !std::isfinite(balance + amount)) {
        return false;
    }
    balance += amount;
    return true;
}

bool Wallet::withdraw(double amount) {
    if (!std::isfinite(amount) || amount <= 0 || !std::isfinite(balance - amount)) {
        return false;
    }
    // Cho phep so du am de viec sua/xoa giao dich cu khong lam hong lich su.
    balance -= amount;
    return true;
}
