#pragma once

#include "../app/money_manager.h"

#include <string>

class ConsoleUI {
private:
    MoneyManager& manager_;

    bool login();
    void dashboard() const;
    void walletMenu();
    void categoryMenu();
    void transactionMenu();
    void budgetMenu();
    void reportMenu() const;
    void accountMenu();

    void printWallets() const;
    void printCategories() const;
    int chooseCategory(CategoryType type, bool allowAll) const;
    void printTransactions() const;
    void printBudgets() const;

    Transaction* inputTransaction(int id) const;
    bool inputBudget(double& limit, std::string& startDate,
                     std::string& endDate, int& categoryId) const;
    bool save() const;

public:
    explicit ConsoleUI(MoneyManager& manager) : manager_(manager) {}
    int run();
};
