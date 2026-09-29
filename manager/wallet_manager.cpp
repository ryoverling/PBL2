#include "wallet_manager.h"

bool WalletManager::addWallet(const Wallet& wallet) {
    if (findById(wallet.getId()) != nullptr) return false;
    wallets.pushBack(wallet);
    return true;
}

int WalletManager::createWallet(string name, WalletType type, double initialBalance) {
    int id = getNextId();
    wallets.pushBack(Wallet(id, name, type, initialBalance));
    return id;
}

bool WalletManager::deleteWallet(int id) {
    return wallets.removeIf([id](const Wallet& w) {
        return w.getId() == id;
    });
}

bool WalletManager::updateWalletName(int id, string newName) {
    Wallet* wallet = findById(id);
    if (wallet == nullptr || newName.empty()) return false;
    wallet->setName(newName);
    return true;
}

Wallet* WalletManager::findById(int id) {
    return wallets.findIf([id](const Wallet& w) {
        return w.getId() == id;
    });
}

const Wallet* WalletManager::findById(int id) const {
    return wallets.findIf([id](const Wallet& w) {
        return w.getId() == id;
    });
}

double WalletManager::getTotalBalance() const {
    double total = 0;
    wallets.forEach([&total](const Wallet& w) {
        total += w.getBalance();
    });
    return total;
}

int WalletManager::getNextId() const {
    int maxId = 0;
    wallets.forEach([&maxId](const Wallet& w) {
        if (w.getId() > maxId) maxId = w.getId();
    });
    return maxId + 1;
}

void WalletManager::clear() {
    wallets.clear();
}

LinkedList<Wallet>& WalletManager::getWallets() {
    return wallets;
}

const LinkedList<Wallet>& WalletManager::getWallets() const {
    return wallets;
}
