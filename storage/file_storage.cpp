#include "file_storage.h"

#include "../app/money_manager.h"

#include <fstream>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

const char* USER_FILE = "data/user.dat";
const char* WALLET_FILE = "data/wallets.dat";
const char* CATEGORY_FILE = "data/categories.dat";
const char* TRANSACTION_FILE = "data/transactions.dat";
const char* BUDGET_FILE = "data/budgets.dat";

bool isValidUtf8(const std::string& text) {
    std::size_t position = 0;
    while (position < text.size()) {
        const unsigned char first = static_cast<unsigned char>(text[position]);
        if (first < 0x80) {
            position++;
            continue;
        }

        std::size_t size = 0;
        if (first >= 0xC2 && first <= 0xDF) size = 2;
        else if (first >= 0xE0 && first <= 0xEF) size = 3;
        else if (first >= 0xF0 && first <= 0xF4) size = 4;
        else return false;

        if (position + size > text.size()) return false;
        for (std::size_t i = 1; i < size; i++) {
            const unsigned char next = static_cast<unsigned char>(text[position + i]);
            if ((next & 0xC0) != 0x80) return false;
        }
        position += size;
    }
    return true;
}

std::string loadText(const std::string& text) {
    if (isValidUtf8(text)) return text;
#ifdef _WIN32
    const int wideSize = MultiByteToWideChar(850, 0, text.c_str(),
                                              static_cast<int>(text.size()), nullptr, 0);
    if (wideSize <= 0) return text;

    std::wstring wideText(static_cast<std::size_t>(wideSize), L'\0');
    MultiByteToWideChar(850, 0, text.c_str(), static_cast<int>(text.size()),
                        &wideText[0], wideSize);

    const int utf8Size = WideCharToMultiByte(CP_UTF8, 0, wideText.c_str(), wideSize,
                                              nullptr, 0, nullptr, nullptr);
    if (utf8Size <= 0) return text;

    std::string utf8Text(static_cast<std::size_t>(utf8Size), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wideText.c_str(), wideSize,
                        &utf8Text[0], utf8Size, nullptr, nullptr);
    return utf8Text;
#else
    return text;
#endif
}

std::string cleanText(std::string value) {
    for (std::size_t i = 0; i < value.size(); i++) {
        if (value[i] == '|' || value[i] == '\n' || value[i] == '\r') value[i] = ' ';
    }
    return value;
}

bool splitLine(const std::string& line, std::string fields[], int fieldCount) {
    std::size_t start = 0;
    for (int i = 0; i < fieldCount; i++) {
        std::size_t separator = line.find('|', start);
        if (i == fieldCount - 1) {
            if (separator != std::string::npos) return false;
            fields[i] = line.substr(start);
            return true;
        }
        if (separator == std::string::npos) return false;
        fields[i] = line.substr(start, separator - start);
        start = separator + 1;
    }
    return false;
}

bool readInt(const std::string& text, int& value) {
    std::istringstream input(text);
    char extra = 0;
    return (input >> value) && !(input >> extra);
}

bool readDouble(const std::string& text, double& value) {
    std::istringstream input(text);
    char extra = 0;
    return (input >> value) && !(input >> extra);
}

bool loadUser(MoneyManager& manager) {
    std::ifstream file(USER_FILE);
    std::string line;
    std::string fields[3];
    if (!std::getline(file, line) || !splitLine(line, fields, 3)) return false;
    manager.setUser(User(loadText(fields[0]), loadText(fields[1]), loadText(fields[2])));
    return !fields[0].empty() && !fields[1].empty();
}

bool loadWallets(MoneyManager& manager) {
    std::ifstream file(WALLET_FILE);
    if (!file) return false;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::string fields[4];
        int id = 0;
        int type = 0;
        double balance = 0;
        if (!splitLine(line, fields, 4) || !readInt(fields[0], id) ||
            !readInt(fields[2], type) || !readDouble(fields[3], balance) || type < 1 || type > 3) {
            return false;
        }
        Wallet wallet(id, loadText(fields[1]), static_cast<WalletType>(type), balance);
        if (!manager.addWallet(wallet)) return false;
    }
    return true;
}

bool loadCategories(MoneyManager& manager) {
    std::ifstream file(CATEGORY_FILE);
    if (!file) return false;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::string fields[3];
        int id = 0;
        int type = 0;
        if (!splitLine(line, fields, 3) || !readInt(fields[0], id) ||
            !readInt(fields[2], type) || type < 1 || type > 2) return false;
        Category category(id, loadText(fields[1]), static_cast<CategoryType>(type));
        if (!manager.addCategory(category)) return false;
    }
    return true;
}

bool loadTransactions(MoneyManager& manager) {
    std::ifstream file(TRANSACTION_FILE);
    if (!file) return false;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::string fields[7];
        int id = 0;
        int type = 0;
        int walletId = 0;
        int categoryId = 0;
        double amount = 0;
        if (!splitLine(line, fields, 7) || !readInt(fields[0], id) ||
            !readInt(fields[1], type) || !readDouble(fields[2], amount) ||
            !readInt(fields[5], walletId) || !readInt(fields[6], categoryId) ||
            type < 1 || type > 2) return false;

        Transaction* transaction = nullptr;
        if (type == 1) transaction = new Income(id, amount, fields[3], loadText(fields[4]), walletId, categoryId);
        else transaction = new Expense(id, amount, fields[3], loadText(fields[4]), walletId, categoryId);

        if (!manager.addTransaction(transaction)) {
            delete transaction;
            return false;
        }
    }
    return true;
}

bool loadBudgets(MoneyManager& manager) {
    std::ifstream file(BUDGET_FILE);
    if (!file) return false;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::string fields[5];
        int id = 0;
        int categoryId = 0;
        double limit = 0;
        if (!splitLine(line, fields, 5) || !readInt(fields[0], id) ||
            !readDouble(fields[1], limit) || !readInt(fields[4], categoryId)) return false;
        Budget budget(id, limit, fields[2], fields[3], categoryId);
        if (!manager.addBudget(budget)) return false;
    }
    return true;
}

} // namespace

bool FileStorage::hasData() {
    std::ifstream file(USER_FILE);
    return file.good();
}

bool FileStorage::load(MoneyManager& manager) {
    manager.clear();
    if (!loadUser(manager)) return false;
    if (!loadWallets(manager)) return false;
    if (!loadCategories(manager)) return false;
    if (!loadTransactions(manager)) return false;
    if (!loadBudgets(manager)) return false;
    return true;
}

bool FileStorage::save(const MoneyManager& manager) {
    std::ofstream userFile(USER_FILE);
    std::ofstream walletFile(WALLET_FILE);
    std::ofstream categoryFile(CATEGORY_FILE);
    std::ofstream transactionFile(TRANSACTION_FILE);
    std::ofstream budgetFile(BUDGET_FILE);
    if (!userFile || !walletFile || !categoryFile || !transactionFile || !budgetFile) return false;

    const User& user = manager.getUser();
    userFile << cleanText(user.getUsername()) << '|' << cleanText(user.getPassword())
             << '|' << cleanText(user.getDisplayName()) << '\n';

    walletFile << std::setprecision(15);
    for (std::size_t i = 0; i < manager.getWallets().getSize(); i++) {
        const Wallet* wallet = manager.getWallets().getAt(i);
        walletFile << wallet->getId() << '|' << cleanText(wallet->getName()) << '|'
                   << static_cast<int>(wallet->getType()) << '|'
                   << wallet->getInitialBalance() << '\n';
    }

    for (std::size_t i = 0; i < manager.getCategories().getSize(); i++) {
        const Category* category = manager.getCategories().getAt(i);
        categoryFile << category->getId() << '|' << cleanText(category->getName()) << '|'
                     << static_cast<int>(category->getType()) << '\n';
    }

    transactionFile << std::setprecision(15);
    for (std::size_t i = 0; i < manager.getTransactions().getSize(); i++) {
        Transaction* const* transaction = manager.getTransactions().getAt(i);
        transactionFile << (*transaction)->getId() << '|'
                        << static_cast<int>((*transaction)->getType()) << '|'
                        << (*transaction)->getAmount() << '|'
                        << (*transaction)->getDate() << '|'
                        << cleanText((*transaction)->getNote()) << '|'
                        << (*transaction)->getWalletId() << '|'
                        << (*transaction)->getCategoryId() << '\n';
    }

    budgetFile << std::setprecision(15);
    for (std::size_t i = 0; i < manager.getBudgets().getSize(); i++) {
        const Budget* budget = manager.getBudgets().getAt(i);
        budgetFile << budget->getId() << '|' << budget->getLimit() << '|'
                   << budget->getStartDate() << '|' << budget->getEndDate() << '|'
                   << budget->getCategoryId() << '\n';
    }

    return userFile.good() && walletFile.good() && categoryFile.good() &&
           transactionFile.good() && budgetFile.good();
}
