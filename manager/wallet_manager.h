#pragma once

#include "../model/wallet.h"
#include "../structure/linked_list.h"

class WalletManager {
private:
    LinkedList<Wallet> wallets;

public:
    bool addWallet(const Wallet& wallet);
    int createWallet(string name, WalletType type, double initialBalance);
    bool deleteWallet(int id);
    bool updateWalletName(int id, string newName);

    Wallet* findById(int id);
    const Wallet* findById(int id) const;

    double getTotalBalance() const;
    int getNextId() const;
    void clear();

    LinkedList<Wallet>& getWallets();
    const LinkedList<Wallet>& getWallets() const;
};
