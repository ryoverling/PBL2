#include "wallet_manager.h"
#include "transaction_manager.h"
#include <cmath>
#include <climits>

namespace {
    bool validType(WalletType type) {
        return type == WalletType::CASH || type == WalletType::BANK ||
               type == WalletType::EWALLET || type == WalletType::OTHER;
    }
}

bool WalletManager::addWallet(const Wallet& wallet) {
    if (wallet.getId() <= 0 || wallet.getName().find_first_not_of(" \t\r\n") == string::npos ||
        !validType(wallet.getType()) || !std::isfinite(wallet.getBalance()) ||
        findById(wallet.getId()) != nullptr) return false;
    wallets.pushBack(wallet);
    return true;
}

int WalletManager::createWallet(string name, WalletType type, double initialBalance) {
    int id = getNextId();
    return addWallet(Wallet(id, name, type, initialBalance)) ? id : -1;
}

bool WalletManager::deleteWallet(int id, const TransactionManager& transactions) {
    if (transactions.hasWalletTransactions(id)) return false;
    return wallets.removeIf([id](const Wallet& w) {
        return w.getId() == id;
    });
}

bool WalletManager::updateWalletName(int id, string newName) {
    Wallet* wallet = findById(id);
    if (wallet == nullptr || newName.find_first_not_of(" \t\r\n") == string::npos) return false;
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
    return maxId == INT_MAX ? -1 : maxId + 1;
}

void WalletManager::clear() {
    wallets.clear();
}

bool WalletManager::updateWallet(int id, string name, WalletType type) {
    if (!validType(type) || !updateWalletName(id, name)) return false;
    findById(id)->setType(type);
    return true;
}

void WalletManager::swap(WalletManager& other) {
    wallets.swap(other.wallets);
}

const LinkedList<Wallet>& WalletManager::getWallets() const {
    return wallets;
}
