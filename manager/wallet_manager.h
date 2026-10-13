#pragma once

#include "../model/wallet.h"
#include "../structure/linked_list.h"

class TransactionManager;

class WalletManager {
private:
    LinkedList<Wallet> wallets;

public:
    bool addWallet(const Wallet& wallet);
    int createWallet(string name, WalletType type, double initialBalance);
    bool deleteWallet(int id, const TransactionManager& transactions);
    bool updateWalletName(int id, string newName);
    bool updateWallet(int id, string name, WalletType type);

    Wallet* findById(int id);
    const Wallet* findById(int id) const;

    double getTotalBalance() const;
    int getNextId() const;
    void clear();

    void swap(WalletManager& other);
    const LinkedList<Wallet>& getWallets() const;
};
