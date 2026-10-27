#include "money_manager.h"

#include <climits>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace {

bool hasText(const std::string& value) {
    return value.find_first_not_of(" \t\r\n") != std::string::npos;
}

bool inDateRange(const std::string& date, const std::string& start, const std::string& end) {
    if (!start.empty() && date < start) return false;
    if (!end.empty() && date > end) return false;
    return true;
}

bool validWalletType(WalletType type) {
    return type == WalletType::Cash || type == WalletType::Bank || type == WalletType::EWallet;
}

bool validCategoryType(CategoryType type) {
    return type == CategoryType::Income || type == CategoryType::Expense;
}

bool validTransactionType(TransactionType type) {
    return type == TransactionType::Income || type == TransactionType::Expense;
}

} // namespace

MoneyManager::~MoneyManager() {
    clear();
}

void MoneyManager::initializeDefaults() {
    clear();
    user_ = User("admin", "123456", "Quan tri vien");
    addWallet("Tien mat", WalletType::Cash, 0);
    addCategory("Luong", CategoryType::Income);
    addCategory("Thu nhap khac", CategoryType::Income);
    addCategory("An uong", CategoryType::Expense);
    addCategory("Di lai", CategoryType::Expense);
    addCategory("Hoc tap", CategoryType::Expense);
    addCategory("Mua sam", CategoryType::Expense);
}

void MoneyManager::clear() {
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction** transaction = transactions_.getAt(i);
        delete *transaction;
        *transaction = nullptr;
    }
    transactions_.clear();
    wallets_.clear();
    categories_.clear();
    budgets_.clear();
}

bool MoneyManager::login(const std::string& username, const std::string& password) const {
    return user_.authenticate(username, password);
}

bool MoneyManager::changeDisplayName(const std::string& name) {
    if (!hasText(name)) return false;
    user_.setDisplayName(name);
    return true;
}

bool MoneyManager::changePassword(const std::string& oldPassword, const std::string& newPassword) {
    if (!hasText(newPassword)) return false;
    return user_.changePassword(oldPassword, newPassword);
}

int MoneyManager::addWallet(const std::string& name, WalletType type, double initialBalance) {
    int nextId = 1;
    for (std::size_t i = 0; i < wallets_.getSize(); i++) {
        const Wallet* wallet = wallets_.getAt(i);
        if (wallet->getId() >= nextId && wallet->getId() < INT_MAX) nextId = wallet->getId() + 1;
    }
    if (!addWallet(Wallet(nextId, name, type, initialBalance))) return -1;
    return nextId;
}

bool MoneyManager::addWallet(const Wallet& wallet) {
    if (wallet.getId() <= 0 || !hasText(wallet.getName())) return false;
    if (!validWalletType(wallet.getType())) return false;
    if (!std::isfinite(wallet.getInitialBalance())) return false;
    if (findWallet(wallet.getId()) != nullptr) return false;
    wallets_.pushBack(wallet);
    return true;
}

bool MoneyManager::updateWallet(int id, const std::string& name, WalletType type) {
    Wallet* wallet = findWallet(id);
    if (wallet == nullptr || !hasText(name) || !validWalletType(type)) return false;
    wallet->update(name, type);
    return true;
}

bool MoneyManager::setWalletBalance(int id, double newBalance) {
    Wallet* wallet = findWallet(id);
    if (wallet == nullptr || !std::isfinite(newBalance) || newBalance < 0) return false;

    double transactionChange = 0;
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction* const* transaction = transactions_.getAt(i);
        if ((*transaction)->getWalletId() == id) {
            transactionChange += (*transaction)->getSignedAmount();
        }
    }
    wallet->setInitialBalance(newBalance - transactionChange);
    return true;
}

bool MoneyManager::deleteWallet(int id) {
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction* const* transaction = transactions_.getAt(i);
        if ((*transaction)->getWalletId() == id) return false;
    }
    for (std::size_t i = 0; i < wallets_.getSize(); i++) {
        const Wallet* wallet = wallets_.getAt(i);
        if (wallet->getId() == id) return wallets_.removeAt(i);
    }
    return false;
}

Wallet* MoneyManager::findWallet(int id) {
    for (std::size_t i = 0; i < wallets_.getSize(); i++) {
        Wallet* wallet = wallets_.getAt(i);
        if (wallet->getId() == id) return wallet;
    }
    return nullptr;
}

const Wallet* MoneyManager::findWallet(int id) const {
    for (std::size_t i = 0; i < wallets_.getSize(); i++) {
        const Wallet* wallet = wallets_.getAt(i);
        if (wallet->getId() == id) return wallet;
    }
    return nullptr;
}

double MoneyManager::getWalletBalance(int walletId) const {
    const Wallet* wallet = findWallet(walletId);
    if (wallet == nullptr) return 0;
    double balance = wallet->getInitialBalance();
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction* const* transaction = transactions_.getAt(i);
        if ((*transaction)->getWalletId() == walletId) balance += (*transaction)->getSignedAmount();
    }
    return balance;
}

double MoneyManager::getTotalBalance() const {
    double total = 0;
    for (std::size_t i = 0; i < wallets_.getSize(); i++) {
        total += wallets_.getAt(i)->getInitialBalance();
    }
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction* const* transaction = transactions_.getAt(i);
        total += (*transaction)->getSignedAmount();
    }
    return total;
}

int MoneyManager::addCategory(const std::string& name, CategoryType type) {
    int nextId = 1;
    for (std::size_t i = 0; i < categories_.getSize(); i++) {
        const Category* category = categories_.getAt(i);
        if (category->getId() >= nextId && category->getId() < INT_MAX) nextId = category->getId() + 1;
    }
    if (!addCategory(Category(nextId, name, type))) return -1;
    return nextId;
}

bool MoneyManager::addCategory(const Category& category) {
    if (category.getId() <= 0 || !hasText(category.getName())) return false;
    if (!validCategoryType(category.getType())) return false;
    if (findCategory(category.getId()) != nullptr) return false;
    categories_.pushBack(category);
    return true;
}

bool MoneyManager::renameCategory(int id, const std::string& name) {
    Category* category = findCategory(id);
    if (category == nullptr || !hasText(name)) return false;
    category->rename(name);
    return true;
}

bool MoneyManager::deleteCategory(int id) {
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction* const* transaction = transactions_.getAt(i);
        if ((*transaction)->getCategoryId() == id) return false;
    }
    for (std::size_t i = 0; i < budgets_.getSize(); i++) {
        if (budgets_.getAt(i)->getCategoryId() == id) return false;
    }
    for (std::size_t i = 0; i < categories_.getSize(); i++) {
        if (categories_.getAt(i)->getId() == id) return categories_.removeAt(i);
    }
    return false;
}

Category* MoneyManager::findCategory(int id) {
    for (std::size_t i = 0; i < categories_.getSize(); i++) {
        Category* category = categories_.getAt(i);
        if (category->getId() == id) return category;
    }
    return nullptr;
}

const Category* MoneyManager::findCategory(int id) const {
    for (std::size_t i = 0; i < categories_.getSize(); i++) {
        const Category* category = categories_.getAt(i);
        if (category->getId() == id) return category;
    }
    return nullptr;
}

bool MoneyManager::validTransaction(const Transaction* transaction) const {
    if (transaction == nullptr || transaction->getId() <= 0) return false;
    if (!validTransactionType(transaction->getType())) return false;
    if (!std::isfinite(transaction->getAmount()) || transaction->getAmount() <= 0) return false;
    if (!isValidDate(transaction->getDate())) return false;
    if (findWallet(transaction->getWalletId()) == nullptr) return false;

    const Category* category = findCategory(transaction->getCategoryId());
    if (category == nullptr) return false;
    if (transaction->getType() == TransactionType::Income) {
        return category->getType() == CategoryType::Income;
    }
    return category->getType() == CategoryType::Expense;
}

int MoneyManager::addTransaction(TransactionType type, double amount, const std::string& date,
                                 const std::string& note, int walletId, int categoryId) {
    if (!validTransactionType(type)) return -1;

    int nextId = 1;
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction* const* item = transactions_.getAt(i);
        if ((*item)->getId() >= nextId && (*item)->getId() < INT_MAX) nextId = (*item)->getId() + 1;
    }

    Transaction* transaction = nullptr;
    if (type == TransactionType::Income) {
        transaction = new Income(nextId, amount, date, note, walletId, categoryId);
    } else {
        transaction = new Expense(nextId, amount, date, note, walletId, categoryId);
    }

    if (!addTransaction(transaction)) {
        delete transaction;
        return -1;
    }
    return nextId;
}

bool MoneyManager::addTransaction(Transaction* transaction) {
    if (!validTransaction(transaction)) return false;
    if (findTransaction(transaction->getId()) != nullptr) return false;
    transactions_.pushBack(transaction);
    return true;
}

bool MoneyManager::updateTransaction(int id, Transaction* replacement) {
    if (replacement == nullptr || replacement->getId() != id) return false;
    if (!validTransaction(replacement)) return false;

    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction** current = transactions_.getAt(i);
        if ((*current)->getId() == id) {
            if (*current == replacement) return true;
            delete *current;
            *current = replacement;
            return true;
        }
    }
    return false;
}

bool MoneyManager::deleteTransaction(int id) {
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction** transaction = transactions_.getAt(i);
        if ((*transaction)->getId() == id) {
            delete *transaction;
            *transaction = nullptr;
            return transactions_.removeAt(i);
        }
    }
    return false;
}

Transaction* MoneyManager::findTransaction(int id) {
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction** transaction = transactions_.getAt(i);
        if ((*transaction)->getId() == id) return *transaction;
    }
    return nullptr;
}

const Transaction* MoneyManager::findTransaction(int id) const {
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction* const* transaction = transactions_.getAt(i);
        if ((*transaction)->getId() == id) return *transaction;
    }
    return nullptr;
}

int MoneyManager::addBudget(double limit, const std::string& startDate,
                            const std::string& endDate, int categoryId) {
    int nextId = 1;
    for (std::size_t i = 0; i < budgets_.getSize(); i++) {
        const Budget* budget = budgets_.getAt(i);
        if (budget->getId() >= nextId && budget->getId() < INT_MAX) nextId = budget->getId() + 1;
    }
    if (!addBudget(Budget(nextId, limit, startDate, endDate, categoryId))) return -1;
    return nextId;
}

bool MoneyManager::addBudget(const Budget& budget) {
    if (budget.getId() <= 0 || findBudget(budget.getId()) != nullptr) return false;
    if (!std::isfinite(budget.getLimit()) || budget.getLimit() <= 0) return false;
    if (!isValidDate(budget.getStartDate()) || !isValidDate(budget.getEndDate())) return false;
    if (budget.getStartDate() > budget.getEndDate()) return false;
    if (budget.getCategoryId() != 0) {
        const Category* category = findCategory(budget.getCategoryId());
        if (category == nullptr || category->getType() != CategoryType::Expense) return false;
    }
    budgets_.pushBack(budget);
    return true;
}

bool MoneyManager::updateBudget(int id, double limit, const std::string& startDate,
                                const std::string& endDate, int categoryId) {
    Budget* budget = findBudget(id);
    if (budget == nullptr || !std::isfinite(limit) || limit <= 0) return false;
    if (!isValidDate(startDate) || !isValidDate(endDate) || startDate > endDate) return false;
    if (categoryId != 0) {
        const Category* category = findCategory(categoryId);
        if (category == nullptr || category->getType() != CategoryType::Expense) return false;
    }
    budget->update(limit, startDate, endDate, categoryId);
    return true;
}

bool MoneyManager::deleteBudget(int id) {
    for (std::size_t i = 0; i < budgets_.getSize(); i++) {
        if (budgets_.getAt(i)->getId() == id) return budgets_.removeAt(i);
    }
    return false;
}

Budget* MoneyManager::findBudget(int id) {
    for (std::size_t i = 0; i < budgets_.getSize(); i++) {
        Budget* budget = budgets_.getAt(i);
        if (budget->getId() == id) return budget;
    }
    return nullptr;
}

const Budget* MoneyManager::findBudget(int id) const {
    for (std::size_t i = 0; i < budgets_.getSize(); i++) {
        const Budget* budget = budgets_.getAt(i);
        if (budget->getId() == id) return budget;
    }
    return nullptr;
}

double MoneyManager::getBudgetSpent(const Budget& budget) const {
    double total = 0;
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction* const* transaction = transactions_.getAt(i);
        if ((*transaction)->getType() != TransactionType::Expense) continue;
        if (!inDateRange((*transaction)->getDate(), budget.getStartDate(), budget.getEndDate())) continue;
        if (budget.getCategoryId() != 0 && (*transaction)->getCategoryId() != budget.getCategoryId()) continue;
        total += (*transaction)->getAmount();
    }
    return total;
}

double MoneyManager::getTotalIncome(const std::string& startDate, const std::string& endDate) const {
    double total = 0;
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction* const* transaction = transactions_.getAt(i);
        if ((*transaction)->getType() == TransactionType::Income &&
            inDateRange((*transaction)->getDate(), startDate, endDate)) {
            total += (*transaction)->getAmount();
        }
    }
    return total;
}

double MoneyManager::getTotalExpense(const std::string& startDate, const std::string& endDate) const {
    double total = 0;
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction* const* transaction = transactions_.getAt(i);
        if ((*transaction)->getType() == TransactionType::Expense &&
            inDateRange((*transaction)->getDate(), startDate, endDate)) {
            total += (*transaction)->getAmount();
        }
    }
    return total;
}

double MoneyManager::getCategoryExpense(int categoryId, const std::string& startDate,
                                        const std::string& endDate) const {
    double total = 0;
    for (std::size_t i = 0; i < transactions_.getSize(); i++) {
        Transaction* const* transaction = transactions_.getAt(i);
        if ((*transaction)->getType() == TransactionType::Expense &&
            (*transaction)->getCategoryId() == categoryId &&
            inDateRange((*transaction)->getDate(), startDate, endDate)) {
            total += (*transaction)->getAmount();
        }
    }
    return total;
}

bool MoneyManager::isValidDate(const std::string& date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
    for (std::size_t i = 0; i < date.size(); i++) {
        if (i != 4 && i != 7 && (date[i] < '0' || date[i] > '9')) return false;
    }

    int year = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day = std::stoi(date.substr(8, 2));
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year < 1900 || month < 1 || month > 12) return false;
    if (month == 2 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))) {
        daysInMonth[2] = 29;
    }
    return day >= 1 && day <= daysInMonth[month];
}

std::string MoneyManager::currentDate() {
    std::time_t now = std::time(nullptr);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    std::ostringstream output;
    output << std::put_time(&localTime, "%Y-%m-%d");
    return output.str();
}
