#include "wallet.h"

std::string walletTypeToString(WalletType type) {
    switch (type) {
        case WalletType::CASH: return "Tiền mặt";
        case WalletType::BANK: return "Ngân hàng";
        case WalletType::EWALLET: return "Ví điện tử";
        default: return "Khác";
    }
}

WalletType walletTypeFromInt(int typeVal) {
    switch (typeVal) {
        case 1: return WalletType::CASH;
        case 2: return WalletType::BANK;
        case 3: return WalletType::EWALLET;
        default: return WalletType::OTHER;
    }
}

Wallet::Wallet() : id(0), name(""), type(WalletType::CASH), balance(0) {}

Wallet::Wallet(int id, std::string name, WalletType type, long long balance)
    : id(id), name(name), type(type), balance(balance) {}

int Wallet::getId() const { return id; }
std::string Wallet::getName() const { return name; }
WalletType Wallet::getType() const { return type; }
long long Wallet::getBalance() const { return balance; }

void Wallet::setName(const std::string& newName) { name = newName; }
void Wallet::setType(WalletType newType) { type = newType; }

bool Wallet::deposit(long long amount) {
    if (amount <= 0) return false;
    balance += amount;
    return true;
}

bool Wallet::withdraw(long long amount) {
    if (amount <= 0) return false;
    balance -= amount;
    return true;
}
