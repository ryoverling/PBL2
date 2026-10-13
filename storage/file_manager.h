#pragma once

#include <string>
#include "../model/user.h"
#include "../manager/category_manager.h"
#include "../manager/wallet_manager.h"
#include "../manager/transaction_manager.h"
#include "../manager/budget_manager.h"
using namespace std;

class FileManager {
private:
    static string sanitize(const string& value);

public:
    static bool ensureDataDirectory(const string& directory = "data");

    static bool loadUser(const string& path, User& user);
    static bool saveUser(const string& path, const User& user);

    static bool loadCategories(const string& path, CategoryManager& manager);
    static bool saveCategories(const string& path, const CategoryManager& manager);

    static bool loadWallets(const string& path, WalletManager& manager);
    static bool saveWallets(const string& path, const WalletManager& manager);

    static bool loadTransactions(const string& path, TransactionManager& manager);
    static bool saveTransactions(const string& path, const TransactionManager& manager);

    static bool loadBudgets(const string& path, BudgetManager& manager);
    static bool saveBudgets(const string& path, const BudgetManager& manager);
};
