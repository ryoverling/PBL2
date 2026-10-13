#include "file_manager.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <locale>
#include <stdexcept>

#include "../model/income.h"
#include "../model/expense.h"

namespace {
    // Fixed-size records retain the final empty field (an empty note).
    bool splitFields(const string& line, string fields[], int count) {
        size_t start = 0;
        for (int i = 0; i < count; ++i) {
            size_t end = line.find('|', start);
            if (i == count - 1) {
                if (end != string::npos) return false;
                fields[i] = line.substr(start);
                return true;
            }
            if (end == string::npos) return false;
            fields[i] = line.substr(start, end - start);
            start = end + 1;
        }
        return false;
    }

    int readInt(const string& value) {
        size_t used = 0;
        int result = stoi(value, &used);
        if (used != value.size()) throw invalid_argument("Invalid integer");
        return result;
    }

    double readAmount(const string& value) {
        size_t used = 0;
        double result = stod(value, &used);
        if (used != value.size() || !std::isfinite(result)) throw invalid_argument("Invalid amount");
        return result;
    }

    // Write completely before replacing the old file. Preserve its backup
    // when recovery fails, rather than silently discarding existing data.
    class FileWriter {
        filesystem::path target;
        filesystem::path temporary;
        filesystem::path backup;
        bool opened = false;
    public:
        ofstream stream;

        explicit FileWriter(const string& path)
            : target(filesystem::u8path(path)), temporary(filesystem::u8path(path + ".tmp")),
              backup(filesystem::u8path(path + ".bak")) {
            error_code error;
            if (filesystem::exists(backup, error) || error) return;
            bool exists = filesystem::exists(target, error);
            if (error || (exists && !filesystem::is_regular_file(target, error)) || error) return;
            stream.open(temporary, ios::trunc);
            opened = stream.is_open();
            stream.imbue(locale::classic());
            stream << setprecision(numeric_limits<double>::max_digits10);
        }

        bool commit() {
            if (!stream.is_open()) return false;
            stream.flush();
            bool written = stream.good();
            stream.close();
            if (!written || stream.fail()) return false;

            error_code error;
            bool hadOriginal = filesystem::exists(target, error);
            if (error) return false;
            if (hadOriginal) {
                filesystem::rename(target, backup, error);
                if (error) return false;
            }
            filesystem::rename(temporary, target, error);
            if (error) {
                if (hadOriginal) {
                    error_code recoveryError;
                    filesystem::rename(backup, target, recoveryError);
                }
                return false;
            }
            if (hadOriginal) filesystem::remove(backup, error);
            return !error;
        }

        ~FileWriter() {
            if (stream.is_open()) stream.close();
            if (opened) {
                error_code error;
                filesystem::remove(temporary, error);
            }
        }
    };
}

string FileManager::sanitize(const string& value) {
    string result = value;
    for (char& c : result) {
        if (c == '|' || c == '\n' || c == '\r') c = ' ';
    }
    return result;
}

bool FileManager::ensureDataDirectory(const string& directory) {
    error_code error;
    filesystem::create_directories(filesystem::u8path(directory), error);
    return !error;
}

bool FileManager::loadUser(const string& path, User& user) {
    ifstream file(filesystem::u8path(path));
    string line, p[3];
    if (!getline(file, line) || !splitFields(line, p, 3) || p[0].empty() || p[1].empty()) return false;
    user = User(p[0], p[1], p[2]);
    return true;
}

bool FileManager::saveUser(const string& path, const User& user) {
    FileWriter file(path);
    file.stream << sanitize(user.getUsername()) << '|'
                << sanitize(user.getPassword()) << '|'
                << sanitize(user.getName()) << '\n';
    return file.commit();
}

bool FileManager::loadCategories(const string& path, CategoryManager& manager) {
    ifstream file(filesystem::u8path(path));
    if (!file) return false;
    CategoryManager loaded;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        string p[3];
        if (!splitFields(line, p, 3) || (p[2] != "INCOME" && p[2] != "EXPENSE")) return false;
        try {
            if (!loaded.addCategory(Category(readInt(p[0]), p[1], categoryTypeFromString(p[2])))) return false;
        } catch (const exception&) { return false; }
    }
    if (!file.eof()) return false;
    manager.swap(loaded);
    return true;
}

bool FileManager::saveCategories(const string& path, const CategoryManager& manager) {
    FileWriter file(path);
    manager.getCategories().forEach([&](const Category& c) {
        file.stream << c.getId() << '|' << sanitize(c.getName()) << '|'
                    << categoryTypeToString(c.getType()) << '\n';
    });
    return file.commit();
}

bool FileManager::loadWallets(const string& path, WalletManager& manager) {
    ifstream file(filesystem::u8path(path));
    if (!file) return false;
    WalletManager loaded;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        string p[4];
        if (!splitFields(line, p, 4)) return false;
        if (p[2] != "CASH" && p[2] != "BANK" && p[2] != "EWALLET" && p[2] != "OTHER") return false;
        try {
            if (!loaded.addWallet(Wallet(readInt(p[0]), p[1], walletTypeFromString(p[2]), readAmount(p[3])))) return false;
        } catch (const exception&) { return false; }
    }
    if (!file.eof()) return false;
    manager.swap(loaded);
    return true;
}

bool FileManager::saveWallets(const string& path, const WalletManager& manager) {
    FileWriter file(path);
    manager.getWallets().forEach([&](const Wallet& w) {
        file.stream << w.getId() << '|' << sanitize(w.getName()) << '|'
                    << walletTypeToString(w.getType()) << '|' << w.getBalance() << '\n';
    });
    return file.commit();
}

bool FileManager::loadTransactions(const string& path, TransactionManager& manager) {
    ifstream file(filesystem::u8path(path));
    if (!file) return false;
    TransactionManager loaded;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        string p[7];
        if (!splitFields(line, p, 7)) return false;
        try {
            int id = readInt(p[0]);
            double amount = readAmount(p[2]);
            int walletId = readInt(p[3]);
            int categoryId = readInt(p[4]);
            Transaction* transaction = nullptr;
            if (p[1] == "INCOME") transaction = new Income(id, amount, p[5], p[6], walletId, categoryId);
            else if (p[1] == "EXPENSE") transaction = new Expense(id, amount, p[5], p[6], walletId, categoryId);
            else return false;
            if (!loaded.addLoadedTransaction(transaction)) {
                delete transaction;
                return false;
            }
        } catch (const exception&) { return false; }
    }
    if (!file.eof()) return false;
    manager.swap(loaded);
    return true;
}

bool FileManager::saveTransactions(const string& path, const TransactionManager& manager) {
    FileWriter file(path);
    manager.getTransactions().forEach([&](Transaction* const& t) {
        file.stream << t->getId() << '|' << transactionTypeToString(t->getType()) << '|'
                    << t->getAmount() << '|' << t->getWalletId() << '|' << t->getCategoryId() << '|'
                    << sanitize(t->getDateTime()) << '|' << sanitize(t->getNote()) << '\n';
    });
    return file.commit();
}

bool FileManager::loadBudgets(const string& path, BudgetManager& manager) {
    ifstream file(filesystem::u8path(path));
    if (!file) return false;
    BudgetManager loaded;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        string p[5];
        if (!splitFields(line, p, 5)) return false;
        try {
            if (!loaded.addBudget(Budget(readInt(p[0]), readAmount(p[1]), p[2], p[3], readInt(p[4])))) return false;
        } catch (const exception&) { return false; }
    }
    if (!file.eof()) return false;
    manager.swap(loaded);
    return true;
}

bool FileManager::saveBudgets(const string& path, const BudgetManager& manager) {
    FileWriter file(path);
    manager.getBudgets().forEach([&](const Budget& b) {
        file.stream << b.getId() << '|' << b.getAmount() << '|'
                    << b.getStartDate() << '|' << b.getEndDate() << '|' << b.getCategoryId() << '\n';
    });
    return file.commit();
}
