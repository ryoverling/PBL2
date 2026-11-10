#include "money_manager.h"
#include "../storage/file_storage.h"

MoneyManager::MoneyManager() = default;

void MoneyManager::clearAll() {
    walletManager.clear();
    categoryManager.clear();
    transactionManager.clear();
}

bool MoneyManager::loadUsers(const std::string& baseDataDir) {
    return userManager.loadUsers(baseDataDir + "/users.dat");
}

bool MoneyManager::saveUsers(const std::string& baseDataDir) const {
    return userManager.saveUsers(baseDataDir + "/users.dat");
}

bool MoneyManager::login(const std::string& username, const std::string& password) {
    bool ok = userManager.login(username, password);
    if (ok) {
        currentUsername = CategoryManager::normalizeName(username);
    }
    return ok;
}

bool MoneyManager::registerUser(const std::string& username, const std::string& password, const std::string& name) {
    return userManager.registerUser(username, password, name);
}

bool MoneyManager::changePassword(const std::string& username, const std::string& oldPassword, const std::string& newPassword) {
    return userManager.changePassword(username, oldPassword, newPassword);
}

void MoneyManager::logout() {
    currentUsername.clear();
    clearAll();
}

const std::string& MoneyManager::getCurrentUsername() const {
    return currentUsername;
}

void MoneyManager::setCurrentUsername(const std::string& username) {
    currentUsername = username;
}

std::string MoneyManager::getUserDataDir(const std::string& baseDataDir, const std::string& username) {
    return baseDataDir + "/" + CategoryManager::normalizeName(username);
}

bool MoneyManager::loadAll(const std::string& userDir) {
    clearAll();
    bool w = FileStorage::loadWallets(userDir + "/wallets.dat", walletManager.getWallets());
    bool c = FileStorage::loadCategories(userDir + "/categories.dat", categoryManager.getCategories());
    bool t = FileStorage::loadTransactions(userDir + "/transactions.dat", transactionManager.getTransactions());
    return w && c && t;
}

bool MoneyManager::saveAll(const std::string& userDir) const {
    bool w = FileStorage::saveWallets(userDir + "/wallets.dat", walletManager.getWallets());
    bool c = FileStorage::saveCategories(userDir + "/categories.dat", categoryManager.getCategories());
    bool t = FileStorage::saveTransactions(userDir + "/transactions.dat", transactionManager.getTransactions());
    return w && c && t;
}

bool MoneyManager::addWallet(const std::string& name, WalletType type, long long initialBalance, int* outId) {
    return walletManager.addWallet(name, type, initialBalance, outId);
}

bool MoneyManager::updateWallet(int id, const std::string& name, WalletType type) {
    return walletManager.updateWallet(id, name, type);
}

bool MoneyManager::deleteWallet(int id) {
    bool isUsed = transactionManager.isWalletUsed(id);
    return walletManager.deleteWallet(id, isUsed);
}

bool MoneyManager::addCategory(const std::string& name, CategoryType type, int* outId) {
    return categoryManager.addCategory(name, type, outId);
}

bool MoneyManager::updateCategory(int id, const std::string& newName, CategoryType newType) {
    bool isUsed = transactionManager.isCategoryUsed(id);
    return categoryManager.updateCategory(id, newName, newType, isUsed);
}

bool MoneyManager::deleteCategory(int id) {
    bool isUsed = transactionManager.isCategoryUsed(id);
    return categoryManager.deleteCategory(id, isUsed);
}

bool MoneyManager::getFilteredTransactions(LinkedList<Transaction*>& result,
                                           const Date& from, const Date& to,
                                           int walletId, int categoryId, int type) const {
    result.clear();
    if (from.isValid() && to.isValid() && to < from) return false;
    transactionManager.filterTransactions(result, from, to, walletId, categoryId, type);
    TransactionManager::sortByDate(result, false);
    return true;
}

WalletManager& MoneyManager::getWalletManager() { return walletManager; }
const WalletManager& MoneyManager::getWalletManager() const { return walletManager; }

CategoryManager& MoneyManager::getCategoryManager() { return categoryManager; }
const CategoryManager& MoneyManager::getCategoryManager() const { return categoryManager; }

TransactionManager& MoneyManager::getTransactionManager() { return transactionManager; }
const TransactionManager& MoneyManager::getTransactionManager() const { return transactionManager; }

bool MoneyManager::addTransaction(Transaction* tx, const std::string& categoryName, CategoryType categoryType) {
    if (!tx || tx->getAmount() <= 0 || !tx->getDate().isValid()) return false;
    Wallet* wallet = walletManager.findById(tx->getWalletId());
    if (!wallet || CategoryManager::normalizeName(categoryName).empty()) return false;
    CategoryType expectedType = tx->getType() == TransactionType::INCOME
                              ? CategoryType::INCOME : CategoryType::EXPENSE;
    if (categoryType != expectedType) return false;

    int catId = categoryManager.findOrCreateCategory(categoryName, categoryType);
    if (catId <= 0) return false;
    const Category* category = categoryManager.findById(catId);
    if (!category) return false;

    tx->setCategoryId(catId);
    return transactionManager.addTransaction(tx, wallet, category);
}

bool MoneyManager::updateTransaction(int id, long long newAmount, const Date& newDate,
                                     const std::string& newNote, int newWalletId,
                                     const std::string& newCategoryName, CategoryType newCatType) {
    if (newAmount <= 0 || !newDate.isValid()) return false;
    if (CategoryManager::normalizeName(newCategoryName).empty()) return false;
    Transaction* oldTx = transactionManager.findById(id);
    if (!oldTx) return false;
    CategoryType expectedType = oldTx->getType() == TransactionType::INCOME
                              ? CategoryType::INCOME : CategoryType::EXPENSE;
    if (newCatType != expectedType) return false;
    Wallet* oldWallet = walletManager.findById(oldTx->getWalletId());
    Wallet* newWallet = walletManager.findById(newWalletId);
    if (!oldWallet || !newWallet) return false;
    int newCategoryId = categoryManager.findOrCreateCategory(newCategoryName, newCatType);
    if (newCategoryId <= 0) return false;
    const Category* newCategory = categoryManager.findById(newCategoryId);
    if (!newCategory) return false;
    return transactionManager.updateTransaction(id, newAmount, newDate, newNote,
                                                newWalletId, newCategoryId,
                                                oldWallet, newWallet, newCategory);
}

bool MoneyManager::deleteTransaction(int id) {
    Transaction* tx = transactionManager.findById(id);
    if (!tx) return false;
    Wallet* wallet = walletManager.findById(tx->getWalletId());
    if (!wallet) return false;
    return transactionManager.deleteTransaction(id, wallet);
}

BalanceValidationResult MoneyManager::validateAddTransaction(const Transaction* tx) const {
    if (!tx) return BalanceValidationResult();
    const Wallet* wallet = walletManager.findById(tx->getWalletId());
    return transactionManager.validateAdd(tx, wallet);
}

BalanceValidationResult MoneyManager::validateUpdateTransaction(int id, long long newAmount, const Date& newDate,
                                                                int newWalletId, const std::string& newCategoryName,
                                                                CategoryType newCatType) const {
    const Transaction* oldTx = transactionManager.findById(id);
    if (!oldTx) return BalanceValidationResult();
    const Wallet* oldWallet = walletManager.findById(oldTx->getWalletId());
    const Wallet* newWallet = walletManager.findById(newWalletId);
    if (!oldWallet || !newWallet) return BalanceValidationResult();
    int catId = categoryManager.findCategoryId(newCategoryName, newCatType);
    const Category* category = catId > 0 ? categoryManager.findById(catId) : nullptr;
    Category tempCat(catId > 0 ? catId : 1, newCategoryName, newCatType);
    return transactionManager.validateUpdate(id, newAmount, newDate, newWalletId,
                                            oldWallet, newWallet, category ? category : &tempCat);
}

BalanceValidationResult MoneyManager::validateDeleteTransaction(int id) const {
    const Transaction* tx = transactionManager.findById(id);
    if (!tx) return BalanceValidationResult();
    const Wallet* wallet = walletManager.findById(tx->getWalletId());
    return transactionManager.validateDelete(id, wallet);
}

long long MoneyManager::getWalletAvailableBalance(int walletId) const {
    const Wallet* w = walletManager.findById(walletId);
    return w ? w->getBalance() : 0;
}

void MoneyManager::getMonthlyTotals(int month, int year, long long& outIncome, long long& outExpense) const {
    StatUtils::getMonthlyTotals(month, year, transactionManager.getTransactions(), outIncome, outExpense);
}

void MoneyManager::get6MonthsStats(int endMonth, int endYear, MonthStat outStats[6]) const {
    StatUtils::get6MonthsStats(endMonth, endYear, transactionManager.getTransactions(), outStats);
}

void MoneyManager::getExpenseByCategory(int month, int year, LinkedList<CategoryExpense>& outList) const {
    StatUtils::getExpenseByCategory(month, year, transactionManager.getTransactions(), categoryManager.getCategories(), outList);
}

ExpenseComparison MoneyManager::compareExpenseWithPreviousMonth(int month, int year) const {
    return StatUtils::compareExpenseWithPreviousMonth(month, year, transactionManager.getTransactions());
}

DailyExpenseList MoneyManager::getExpenseByDay(int month, int year) const {
    return StatUtils::getExpenseByDay(transactionManager.getTransactions(), month, year);
}

long long MoneyManager::getMonthlyDifference(int month, int year) const {
    long long income = 0, expense = 0;
    getMonthlyTotals(month, year, income, expense);
    return income - expense;
}
