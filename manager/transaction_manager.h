#pragma once

#include "../model/transaction.h"
#include "../model/income.h"
#include "../model/expense.h"
#include "../structure/linked_list.h"
#include "wallet_manager.h"
#include "category_manager.h"

class TransactionManager {
private:
    LinkedList<Transaction*> transactions;

    bool validateTransaction(const Transaction* transaction,
                             const WalletManager& walletManager,
                             const CategoryManager& categoryManager) const;

    bool applyWalletEffect(const Transaction* transaction,
                           WalletManager& walletManager,
                           bool reverse);

public:
    TransactionManager() = default;
    ~TransactionManager();

    TransactionManager(const TransactionManager&) = delete;
    TransactionManager& operator=(const TransactionManager&) = delete;

    // Ownership transfers to this manager only when add/update succeeds.
    bool addTransaction(Transaction* transaction,
                        WalletManager& walletManager,
                        const CategoryManager& categoryManager,
                        bool applyWallet = true);

    bool addLoadedTransaction(Transaction* transaction);

    bool deleteTransaction(int id, WalletManager& walletManager);

    bool updateTransaction(int id,
                           Transaction* replacement,
                           WalletManager& walletManager,
                           const CategoryManager& categoryManager);

    Transaction* findById(int id);
    const Transaction* findById(int id) const;

    double getTotalIncome(string startDate = "", string endDate = "") const;
    double getTotalExpense(string startDate = "", string endDate = "") const;
    double getCategoryExpense(int categoryId, string startDate = "", string endDate = "") const;
    void getTransactionsByDate(LinkedList<const Transaction*>& result) const;
    bool hasValidReferences(const WalletManager& wallets, const CategoryManager& categories) const;
    void swap(TransactionManager& other);

    bool hasWalletTransactions(int walletId) const;
    bool hasCategoryTransactions(int categoryId) const;

    int getNextId() const;
    void clear();

    const LinkedList<Transaction*>& getTransactions() const;
};
