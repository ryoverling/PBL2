#pragma once

#include "../model/budget.h"
#include "../model/category.h"
#include "../model/transaction.h"
#include "../model/user.h"
#include "../model/wallet.h"
#include "../structure/linked_list.h"

#include <string>

class MoneyManager {
private:
    User user_;
    LinkedList<Wallet> wallets_;
    LinkedList<Category> categories_;
    LinkedList<Transaction*> transactions_;
    LinkedList<Budget> budgets_;

    bool validTransaction(const Transaction* transaction) const;

public:
    MoneyManager() = default;
    ~MoneyManager();
    MoneyManager(const MoneyManager&) = delete;
    MoneyManager& operator=(const MoneyManager&) = delete;

    void initializeDefaults();
    void clear();

    const User& getUser() const { return user_; }
    void setUser(const User& user) { user_ = user; }
    bool login(const std::string& username, const std::string& password) const;
    bool changeDisplayName(const std::string& name);
    bool changePassword(const std::string& oldPassword, const std::string& newPassword);

    int addWallet(const std::string& name, WalletType type, double initialBalance);
    bool addWallet(const Wallet& wallet);
    bool updateWallet(int id, const std::string& name, WalletType type);
    bool setWalletBalance(int id, double newBalance);
    bool deleteWallet(int id);
    Wallet* findWallet(int id);
    const Wallet* findWallet(int id) const;
    double getWalletBalance(int walletId) const;
    double getTotalBalance() const;

    int addCategory(const std::string& name, CategoryType type);
    bool addCategory(const Category& category);
    bool renameCategory(int id, const std::string& name);
    bool deleteCategory(int id);
    Category* findCategory(int id);
    const Category* findCategory(int id) const;

    int addTransaction(TransactionType type, double amount, const std::string& date,
                       const std::string& note, int walletId, int categoryId);
    bool addTransaction(Transaction* transaction);
    bool updateTransaction(int id, Transaction* replacement);
    bool deleteTransaction(int id);
    Transaction* findTransaction(int id);
    const Transaction* findTransaction(int id) const;

    int addBudget(double limit, const std::string& startDate,
                  const std::string& endDate, int categoryId);
    bool addBudget(const Budget& budget);
    bool updateBudget(int id, double limit, const std::string& startDate,
                      const std::string& endDate, int categoryId);
    bool deleteBudget(int id);
    Budget* findBudget(int id);
    const Budget* findBudget(int id) const;
    double getBudgetSpent(const Budget& budget) const;

    double getTotalIncome(const std::string& startDate = "",
                          const std::string& endDate = "") const;
    double getTotalExpense(const std::string& startDate = "",
                           const std::string& endDate = "") const;
    double getCategoryExpense(int categoryId, const std::string& startDate,
                              const std::string& endDate) const;

    static bool isValidDate(const std::string& date);
    static std::string currentDate();

    const LinkedList<Wallet>& getWallets() const { return wallets_; }
    const LinkedList<Category>& getCategories() const { return categories_; }
    const LinkedList<Transaction*>& getTransactions() const { return transactions_; }
    const LinkedList<Budget>& getBudgets() const { return budgets_; }
};
