#include "transaction_manager.h"
#include "../utils/date_utils.h"

TransactionManager::~TransactionManager() {
    clear();
}

bool TransactionManager::validateTransaction(const Transaction* transaction,
                                               const WalletManager& walletManager,
                                               const CategoryManager& categoryManager) const {
    if (transaction == nullptr || transaction->getAmount() <= 0) return false;

    const Wallet* wallet = walletManager.findById(transaction->getWalletId());
    const Category* category = categoryManager.findById(transaction->getCategoryId());
    if (wallet == nullptr || category == nullptr) return false;

    if (transaction->getType() == TransactionType::INCOME &&
        category->getType() != CategoryType::INCOME) return false;

    if (transaction->getType() == TransactionType::EXPENSE &&
        category->getType() != CategoryType::EXPENSE) return false;

    return true;
}

bool TransactionManager::applyWalletEffect(const Transaction* transaction,
                                            WalletManager& walletManager,
                                            bool reverse) {
    if (transaction == nullptr) return false;
    Wallet* wallet = walletManager.findById(transaction->getWalletId());
    if (wallet == nullptr) return false;

    double amount = transaction->getAmount();

    if (!reverse) {
        if (transaction->getType() == TransactionType::INCOME) {
            return wallet->deposit(amount);
        }
        return wallet->withdraw(amount);
    }

    // Hoan tac giao dich cu.
    if (transaction->getType() == TransactionType::INCOME) {
        return wallet->withdraw(amount);
    }
    return wallet->deposit(amount);
}

bool TransactionManager::addTransaction(Transaction* transaction,
                                        WalletManager& walletManager,
                                        const CategoryManager& categoryManager,
                                        bool applyWallet) {
    if (transaction == nullptr) return false;
    if (findById(transaction->getId()) != nullptr) return false;
    if (!validateTransaction(transaction, walletManager, categoryManager)) return false;

    if (applyWallet && !applyWalletEffect(transaction, walletManager, false)) {
        return false;
    }

    transactions.pushBack(transaction);
    return true;
}

bool TransactionManager::addLoadedTransaction(Transaction* transaction) {
    if (transaction == nullptr || findById(transaction->getId()) != nullptr) return false;
    transactions.pushBack(transaction);
    return true;
}

bool TransactionManager::deleteTransaction(int id, WalletManager& walletManager) {
    Transaction* transaction = findById(id);
    if (transaction == nullptr) return false;

    if (!applyWalletEffect(transaction, walletManager, true)) return false;

    Transaction* pointerToDelete = transaction;
    bool removed = transactions.removeIf([id](Transaction* const& t) {
        return t != nullptr && t->getId() == id;
    });

    if (removed) {
        delete pointerToDelete;
        return true;
    }
    return false;
}

bool TransactionManager::updateTransaction(int id,
                                           Transaction* replacement,
                                           WalletManager& walletManager,
                                           const CategoryManager& categoryManager) {
    Transaction* oldTransaction = findById(id);
    if (oldTransaction == nullptr || replacement == nullptr) return false;
    if (replacement->getId() != id) return false;
    if (!validateTransaction(replacement, walletManager, categoryManager)) return false;

    if (!applyWalletEffect(oldTransaction, walletManager, true)) return false;

    if (!applyWalletEffect(replacement, walletManager, false)) {
        applyWalletEffect(oldTransaction, walletManager, false);
        return false;
    }

    Transaction* oldPointer = oldTransaction;
    bool removed = transactions.removeIf([id](Transaction* const& t) {
        return t != nullptr && t->getId() == id;
    });

    if (!removed) {
        applyWalletEffect(replacement, walletManager, true);
        applyWalletEffect(oldTransaction, walletManager, false);
        return false;
    }

    delete oldPointer;
    transactions.pushBack(replacement);
    return true;
}

Transaction* TransactionManager::findById(int id) {
    Transaction** result = transactions.findIf([id](Transaction* const& t) {
        return t != nullptr && t->getId() == id;
    });
    return result == nullptr ? nullptr : *result;
}

const Transaction* TransactionManager::findById(int id) const {
    Transaction* const* result = transactions.findIf([id](Transaction* const& t) {
        return t != nullptr && t->getId() == id;
    });
    return result == nullptr ? nullptr : *result;
}

double TransactionManager::getTotalIncome(string startDate, string endDate) const {
    double total = 0;
    transactions.forEach([&](Transaction* const& t) {
        if (t == nullptr || t->getType() != TransactionType::INCOME) return;
        if (!startDate.empty() && !endDate.empty() &&
            !DateUtils::isDateInRange(t->getDateTime(), startDate, endDate)) return;
        total += t->getAmount();
    });
    return total;
}

double TransactionManager::getTotalExpense(string startDate, string endDate) const {
    double total = 0;
    transactions.forEach([&](Transaction* const& t) {
        if (t == nullptr || t->getType() != TransactionType::EXPENSE) return;
        if (!startDate.empty() && !endDate.empty() &&
            !DateUtils::isDateInRange(t->getDateTime(), startDate, endDate)) return;
        total += t->getAmount();
    });
    return total;
}

bool TransactionManager::hasWalletTransactions(int walletId) const {
    bool found = false;
    transactions.forEach([&](Transaction* const& t) {
        if (t != nullptr && t->getWalletId() == walletId) found = true;
    });
    return found;
}

bool TransactionManager::hasCategoryTransactions(int categoryId) const {
    bool found = false;
    transactions.forEach([&](Transaction* const& t) {
        if (t != nullptr && t->getCategoryId() == categoryId) found = true;
    });
    return found;
}

int TransactionManager::getNextId() const {
    int maxId = 0;
    transactions.forEach([&](Transaction* const& t) {
        if (t != nullptr && t->getId() > maxId) maxId = t->getId();
    });
    return maxId + 1;
}

void TransactionManager::clear() {
    transactions.forEach([](Transaction*& t) {
        delete t;
        t = nullptr;
    });
    transactions.clear();
}

LinkedList<Transaction*>& TransactionManager::getTransactions() {
    return transactions;
}

const LinkedList<Transaction*>& TransactionManager::getTransactions() const {
    return transactions;
}
