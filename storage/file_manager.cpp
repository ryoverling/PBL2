#include "file_manager.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

#include "../model/income.h"
#include "../model/expense.h"

namespace {
    vector<string> splitLine(const string& line, char delimiter = '|') {
        vector<string> parts;
        string item;
        stringstream ss(line);
        while (getline(ss, item, delimiter)) {
            parts.push_back(item);
        }
        return parts;
    }
}

string FileManager::sanitize(const string& value) {
    string result = value;
    for (char& c : result) {
        if (c == '|' || c == '\n' || c == '\r') c = ' ';
    }
    return result;
}

void FileManager::ensureDataDirectory(const string& directory) {
    filesystem::create_directories(directory);
}

bool FileManager::loadUser(const string& path, User& user) {
    ifstream file(path);
    if (!file) return false;

    string line;
    if (!getline(file, line)) return false;
    vector<string> p = splitLine(line);
    if (p.size() < 3) return false;

    user = User(p[0], p[1], p[2]);
    return true;
}

bool FileManager::saveUser(const string& path, const User& user) {
    ofstream file(path);
    if (!file) return false;
    file << sanitize(user.getUsername()) << '|'
         << sanitize(user.getPassword()) << '|'
         << sanitize(user.getName()) << '\n';
    return true;
}

bool FileManager::loadCategories(const string& path, CategoryManager& manager) {
    ifstream file(path);
    if (!file) return false;
    manager.clear();

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        vector<string> p = splitLine(line);
        if (p.size() < 3) continue;
        try {
            int id = stoi(p[0]);
            manager.addCategory(Category(id, p[1], categoryTypeFromString(p[2])));
        } catch (...) {}
    }
    return true;
}

bool FileManager::saveCategories(const string& path, const CategoryManager& manager) {
    ofstream file(path);
    if (!file) return false;

    manager.getCategories().forEach([&](const Category& c) {
        file << c.getId() << '|'
             << sanitize(c.getName()) << '|'
             << categoryTypeToString(c.getType()) << '\n';
    });
    return true;
}

bool FileManager::loadWallets(const string& path, WalletManager& manager) {
    ifstream file(path);
    if (!file) return false;
    manager.clear();

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        vector<string> p = splitLine(line);
        if (p.size() < 4) continue;
        try {
            int id = stoi(p[0]);
            double balance = stod(p[3]);
            manager.addWallet(Wallet(id, p[1], walletTypeFromString(p[2]), balance));
        } catch (...) {}
    }
    return true;
}

bool FileManager::saveWallets(const string& path, const WalletManager& manager) {
    ofstream file(path);
    if (!file) return false;

    manager.getWallets().forEach([&](const Wallet& w) {
        file << w.getId() << '|'
             << sanitize(w.getName()) << '|'
             << walletTypeToString(w.getType()) << '|'
             << w.getBalance() << '\n';
    });
    return true;
}

bool FileManager::loadTransactions(const string& path, TransactionManager& manager) {
    ifstream file(path);
    if (!file) return false;
    manager.clear();

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        vector<string> p = splitLine(line);
        if (p.size() < 7) continue;

        try {
            int id = stoi(p[0]);
            double amount = stod(p[2]);
            int walletId = stoi(p[3]);
            int categoryId = stoi(p[4]);

            Transaction* t = nullptr;
            if (p[1] == "INCOME") {
                t = new Income(id, amount, p[5], p[6], walletId, categoryId);
            } else if (p[1] == "EXPENSE") {
                t = new Expense(id, amount, p[5], p[6], walletId, categoryId);
            }

            if (t != nullptr && !manager.addLoadedTransaction(t)) {
                delete t;
            }
        } catch (...) {}
    }
    return true;
}

bool FileManager::saveTransactions(const string& path, const TransactionManager& manager) {
    ofstream file(path);
    if (!file) return false;

    manager.getTransactions().forEach([&](Transaction* const& t) {
        if (t == nullptr) return;
        file << t->getId() << '|'
             << transactionTypeToString(t->getType()) << '|'
             << t->getAmount() << '|'
             << t->getWalletId() << '|'
             << t->getCategoryId() << '|'
             << sanitize(t->getDateTime()) << '|'
             << sanitize(t->getNote()) << '\n';
    });
    return true;
}

bool FileManager::loadBudgets(const string& path, BudgetManager& manager) {
    ifstream file(path);
    if (!file) return false;
    manager.clear();

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        vector<string> p = splitLine(line);
        if (p.size() < 5) continue;
        try {
            manager.addBudget(Budget(stoi(p[0]), stod(p[1]), p[2], p[3], stoi(p[4])));
        } catch (...) {}
    }
    return true;
}

bool FileManager::saveBudgets(const string& path, const BudgetManager& manager) {
    ofstream file(path);
    if (!file) return false;

    manager.getBudgets().forEach([&](const Budget& b) {
        file << b.getId() << '|'
             << b.getAmount() << '|'
             << b.getStartDate() << '|'
             << b.getEndDate() << '|'
             << b.getCategoryId() << '\n';
    });
    return true;
}
