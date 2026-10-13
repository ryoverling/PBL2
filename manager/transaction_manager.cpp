#include "transaction_manager.h"
#include "../utils/date_utils.h"
#include <cmath>
#include <climits>

namespace {
    bool isValidRecord(const Transaction* transaction) {
        return transaction != nullptr && transaction->getId() > 0 &&
            std::isfinite(transaction->getAmount()) && transaction->getAmount() > 0 &&
            transaction->getWalletId() > 0 && transaction->getCategoryId() > 0 &&
            DateUtils::isValidDateTime(transaction->getDateTime());
    }
}

TransactionManager::~TransactionManager() {
    clear();
}

bool TransactionManager::validateTransaction(const Transaction* transaction,
                                               const WalletManager& walletManager,
                                               const CategoryManager& categoryManager) const {
    if (!isValidRecord(transaction)) return false;

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

    // Dynamic dispatch: Income adds money, Expense subtracts money.
    double change = transaction->getSignedAmount();
    if (reverse) change = -change;
    return change > 0 ? wallet->deposit(change) : wallet->withdraw(-change);
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
    if (!isValidRecord(transaction) || findById(transaction->getId()) != nullptr) return false;
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
    if (replacement == oldTransaction) return true;

    if (!applyWalletEffect(oldTransaction, walletManager, true)) return false;

    if (!applyWalletEffect(replacement, walletManager, false)) {
        applyWalletEffect(oldTransaction, walletManager, false);
        return false;
    }

    Transaction** slot = transactions.findIf([id](Transaction* const& t) {
        return t != nullptr && t->getId() == id;
    });
    *slot = replacement;
    delete oldTransaction;
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
        if (!DateUtils::isDateInRange(t->getDateTime(), startDate, endDate)) return;
        total += t->getAmount();
    });
    return total;
}

double TransactionManager::getTotalExpense(string startDate, string endDate) const {
    double total = 0;
    transactions.forEach([&](Transaction* const& t) {
        if (t == nullptr || t->getType() != TransactionType::EXPENSE) return;
        if (!DateUtils::isDateInRange(t->getDateTime(), startDate, endDate)) return;
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
    return maxId == INT_MAX ? -1 : maxId + 1;
}

void TransactionManager::clear() {
    transactions.forEach([](Transaction*& t) {
        delete t;
        t = nullptr;
    });
    transactions.clear();
}

double TransactionManager::getCategoryExpense(int categoryId, string startDate, string endDate) const {
    double total = 0;
    transactions.forEach([&](Transaction* const& t) {
        if (t->getType() == TransactionType::EXPENSE && t->getCategoryId() == categoryId &&
            DateUtils::isDateInRange(t->getDateTime(), startDate, endDate)) total += t->getAmount();
    });
    return total;
}

void TransactionManager::getTransactionsByDate(LinkedList<const Transaction*>& result) const {
    result.clear();
    transactions.forEach([&](Transaction* const& t) { result.pushBack(t); });
    result.sort([](const Transaction* a, const Transaction* b) {
        if (a->getDateTime() == b->getDateTime()) return a->getId() > b->getId();
        return a->getDateTime() > b->getDateTime();
    });
}

bool TransactionManager::hasValidReferences(const WalletManager& wallets, const CategoryManager& categories) const {
    bool valid = true;
    transactions.forEach([&](Transaction* const& t) {
        if (!validateTransaction(t, wallets, categories)) valid = false;
    });
    return valid;
}

void TransactionManager::swap(TransactionManager& other) {
    transactions.swap(other.transactions);
}

const LinkedList<Transaction*>& TransactionManager::getTransactions() const {
    return transactions;
}
