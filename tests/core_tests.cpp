#include "storage/file_manager.h"
#include "utils/date_utils.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace fs = std::filesystem;

namespace {
    int checks = 0;
    void check(bool condition, const char* message) {
        ++checks;
        if (!condition) throw std::runtime_error(message);
    }

    void writeText(const fs::path& path, const std::string& text) {
        std::ofstream file(path);
        file << text;
        file.close();
        check(!file.fail(), "Write test fixture");
    }

    std::string readText(const fs::path& path) {
        std::ifstream file(path);
        return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

    void testList() {
        LinkedList<int> list;
        list.sort([](int a, int b) { return a < b; });
        check(list.isEmpty(), "Sort empty list");
        for (int value : {3, 1, 2, 1}) list.pushBack(value);
        list.sort([](int a, int b) { return a < b; });
        const int expected[] = {1, 1, 2, 3};
        int index = 0;
        const LinkedList<int>& view = list;
        view.forEach([&](const int& value) { check(value == expected[index++], "List sorted order"); });
        check(index == 4 && list.getSize() == 4, "Sort preserves size");
        check(list.removeIf([](int n) { return n == 1; }), "Remove head");
        check(list.removeIf([](int n) { return n == 3; }), "Remove tail");
        list.pushBack(4);
        check(list.getSize() == 3 && *list.findIf([](int n) { return n == 4; }) == 4, "Append after removing tail");
        LinkedList<int> other;
        other.pushBack(9);
        list.swap(other);
        check(list.getSize() == 1 && other.getSize() == 3, "Swap owns correct nodes");
        list.clear();
        list.pushBack(7);
        check(list.removeIf([](int n) { return n == 7; }) && list.isEmpty(), "Remove only node");
        list.pushBack(8);
        check(list.getSize() == 1, "Append after removing only node");
        struct Item { int key; int order; };
        LinkedList<Item> stable;
        stable.pushBack({2, 0}); stable.pushBack({1, 1}); stable.pushBack({2, 2});
        stable.sort([](const Item& a, const Item& b) { return a.key < b.key; });
        int orders[] = {1, 0, 2}; index = 0;
        stable.forEach([&](const Item& value) { check(value.order == orders[index++], "Stable insertion sort"); });
    }

    void testDates() {
        check(DateUtils::isValidDate("2024-02-29"), "Leap day");
        for (const char* date : {"2023-02-29", "2026-04-31", "2026-1x-01", "2026-00-01", "2026-01-1a"}) {
            check(!DateUtils::isValidDate(date), "Reject invalid calendar date");
        }
        check(DateUtils::isValidDateTime("2026-10-13 23:59:59"), "Timestamp");
        check(!DateUtils::isValidDateTime("2026-10-13 24:00:00"), "Reject invalid hour");
        check(!DateUtils::isValidDateTime("2026-10-13 12:60:00"), "Reject invalid minute");
        check(DateUtils::isDateInRange("2026-10-13", "2026-10-13", "2026-10-13"), "Inclusive date bounds");
        check(!DateUtils::isDateInRange("2026-10-12", "2026-10-13", ""), "Open-ended start filter");
        check(!DateUtils::isDateInRange("2026-10-14", "", "2026-10-13"), "Open-ended end filter");
        check(DateUtils::addDays("2024-02-28", 1) == "2024-02-29", "Date arithmetic");
    }

    void testManagers() {
        CategoryManager categories;
        WalletManager wallets;
        BudgetManager budgets;
        TransactionManager transactions;
        int income = categories.createCategory("Salary", CategoryType::INCOME);
        int expense = categories.createCategory("Food", CategoryType::EXPENSE);
        int first = wallets.createWallet("Cash", WalletType::CASH, 1000);
        int second = wallets.createWallet("Bank", WalletType::BANK, 500);
        check(income > 0 && expense > 0 && first > 0 && second > 0, "Create entities");
        check(categories.createCategory("  ", CategoryType::EXPENSE) < 0, "Reject blank category");
        check(wallets.createWallet("", WalletType::CASH, 0) < 0, "Reject blank wallet");
        check(wallets.createWallet("Bad", WalletType::CASH, std::numeric_limits<double>::infinity()) < 0, "Reject infinite balance");
        check(transactions.addTransaction(new Income(1, 200, "2026-10-01", "", first, income), wallets, categories), "Add income");
        check(transactions.addTransaction(new Expense(2, 50, "2026-10-02", "", first, expense), wallets, categories), "Add expense");
        check(wallets.findById(first)->getBalance() == 1150, "Polymorphic wallet effect");
        check(!wallets.deleteWallet(first, transactions), "Prevent deleting referenced wallet in core");
        check(!categories.deleteCategory(expense, transactions, budgets), "Prevent deleting referenced category in core");

        Expense* invalid = new Expense(3, 10, "2026-10-03", "", first, income);
        check(!transactions.addTransaction(invalid, wallets, categories), "Reject mismatched category");
        delete invalid;
        Income* duplicate = new Income(1, 1, "2026-10-01", "", first, income);
        check(!transactions.addTransaction(duplicate, wallets, categories), "Reject duplicate transaction");
        delete duplicate;
        Income* nan = new Income(3, std::numeric_limits<double>::quiet_NaN(), "2026-10-01", "", first, income);
        check(!transactions.addTransaction(nan, wallets, categories), "Reject NaN amount");
        delete nan;
        check(wallets.findById(first)->getBalance() == 1150, "Rejected additions preserve balance");

        Expense* badReplacement = new Expense(2, 100, "2026-10-02", "", 999, expense);
        check(!transactions.updateTransaction(2, badReplacement, wallets, categories), "Reject invalid replacement");
        delete badReplacement;
        check(wallets.findById(first)->getBalance() == 1150 && transactions.findById(2)->getAmount() == 50,
              "Rejected edit preserves balance and transaction");
        check(transactions.updateTransaction(2, new Expense(2, 100, "2026-10-02", "", second, expense), wallets, categories), "Move expense to another wallet");
        check(wallets.findById(first)->getBalance() == 1200 && wallets.findById(second)->getBalance() == 400, "Edit reverses old effect");
        check(transactions.updateTransaction(2, transactions.findById(2), wallets, categories), "Self replacement does not delete owned pointer");
        check(transactions.getTotalIncome() == 200 && transactions.getTotalExpense() == 100, "Income and expense totals");
        check(transactions.getCategoryExpense(expense, "2026-10-01", "2026-10-31") == 100, "Category expense total");
        check(transactions.getTotalIncome("2026-10-02", "") == 0, "Manager applies single date bound");

        int budget = budgets.createBudget(80, "2026-10-01", "2026-10-31", expense, categories);
        check(budget > 0 && budgets.isOverBudget(*budgets.findById(budget), transactions), "Budget overspending");
        check(budgets.calculateRemaining(*budgets.findById(budget), transactions) == -20, "Budget remainder");
        check(budgets.createBudget(100, "2026-02-30", "2026-03-01", expense, categories) < 0, "Reject invalid budget date");
        check(budgets.createBudget(100, "2026-10-01", "2026-10-31", income, categories) < 0, "Reject income category budget");
        check(!budgets.updateBudget(budget, 200, "2026-11-01", "2026-10-31", expense, categories), "Reject reversed budget range");
        check(budgets.findById(budget)->getAmount() == 80, "Rejected budget edit is unchanged");
        check(budgets.updateBudget(budget, 200, "2026-10-01", "2026-10-31", 0, categories), "Update complete budget");

        LinkedList<const Transaction*> sorted;
        transactions.getTransactionsByDate(sorted);
        int expectedId = 2;
        sorted.forEach([&](const Transaction* t) { check(t->getId() == expectedId--, "Recent transactions sorted"); });
        check(transactions.deleteTransaction(2, wallets), "Delete expense");
        check(wallets.findById(second)->getBalance() == 500, "Delete restores wallet");
        int categoryBudget = budgets.createBudget(100, "2026-10-01", "2026-10-31", expense, categories);
        check(!categories.deleteCategory(expense, transactions, budgets), "Budget alone protects category");
        check(budgets.deleteBudget(categoryBudget) && categories.deleteCategory(expense, transactions, budgets), "Delete unreferenced category");
        check(transactions.deleteTransaction(1, wallets) && wallets.findById(first)->getBalance() == 1000, "Delete income restores initial balance");
        check(wallets.deleteWallet(first, transactions), "Delete unreferenced wallet");
    }

    void testStorage(const fs::path& root) {
        fs::create_directories(root);
        std::string categoryPath = (root / "categories.txt").u8string();
        std::string walletPath = (root / "wallets.txt").u8string();
        std::string transactionPath = (root / "transactions.txt").u8string();
        std::string budgetPath = (root / "budgets.txt").u8string();
        CategoryManager categories;
        WalletManager wallets;
        TransactionManager transactions;
        BudgetManager budgets;
        int income = categories.createCategory("Salary", CategoryType::INCOME);
        int expense = categories.createCategory("Food", CategoryType::EXPENSE);
        int wallet = wallets.createWallet("Cash", WalletType::CASH, 12345678.25);
        check(transactions.addTransaction(new Income(1, 12345678, "2026-10-13 10:00:00", "", wallet, income), wallets, categories), "Empty-note fixture");
        check(transactions.addTransaction(new Expense(2, 8765432.125, "2026-10-13 10:00:00", "food", wallet, expense), wallets, categories), "Decimal fixture");
        int budget = budgets.createBudget(98765432.125, "2026-10-01", "2026-10-31", expense, categories);
        check(FileManager::saveCategories(categoryPath, categories) && FileManager::saveWallets(walletPath, wallets) &&
              FileManager::saveTransactions(transactionPath, transactions) && FileManager::saveBudgets(budgetPath, budgets), "Save all entities");
        CategoryManager loadedCategories;
        WalletManager loadedWallets;
        TransactionManager loadedTransactions;
        BudgetManager loadedBudgets;
        check(FileManager::loadCategories(categoryPath, loadedCategories) && FileManager::loadWallets(walletPath, loadedWallets) &&
              FileManager::loadTransactions(transactionPath, loadedTransactions) && FileManager::loadBudgets(budgetPath, loadedBudgets), "Reload all entities");
        check(loadedTransactions.getTransactions().getSize() == 2 && loadedTransactions.findById(1)->getNote().empty(), "Empty note survives reload");
        check(loadedTransactions.findById(1)->getAmount() == 12345678 && loadedTransactions.findById(2)->getAmount() == 8765432.125, "Exact transaction money round trip");
        check(loadedWallets.findById(wallet)->getBalance() == wallets.findById(wallet)->getBalance(), "Exact wallet money round trip without applying transactions twice");
        check(loadedBudgets.findById(budget)->getAmount() == 98765432.125, "Exact budget money round trip");
        check(loadedTransactions.hasValidReferences(loadedWallets, loadedCategories) && loadedBudgets.hasValidReferences(loadedCategories), "Loaded relationships valid");
        check(FileManager::saveTransactions(transactionPath, loadedTransactions), "Replace existing file");
        check(!fs::exists(transactionPath + ".bak") && !fs::exists(transactionPath + ".tmp"), "Successful save cleans temporary files");

        const char* malformed[] = {
            "1|INCOME|5|1|1|2026-10-13|ok\n2|EXPENSE|nan|1|2|2026-10-13|bad\n",
            "1|INCOME|5|1|1|2026-10-13|\n1|INCOME|5|1|1|2026-10-13|\n",
            "1|INCOME|5oops|1|1|2026-10-13|\n",
            "1|INCOME|-5|1|1|2026-10-13|\n",
            "1|INCOME|5|1|1|2026-02-30|\n",
            "1|UNKNOWN|5|1|1|2026-10-13|\n",
            "1|INCOME|5|1|1|2026-10-13\n"
        };
        for (const char* text : malformed) {
            writeText(transactionPath, text);
            check(!FileManager::loadTransactions(transactionPath, loadedTransactions), "Reject malformed file");
            check(loadedTransactions.getTransactions().getSize() == 2 && loadedTransactions.findById(1)->getAmount() == 12345678,
                  "Failed load preserves all in-memory records");
            check(readText(transactionPath) == text, "Failed load does not rewrite file");
        }
        writeText(walletPath, "1|Cash|CASH|inf\n");
        check(!FileManager::loadWallets(walletPath, loadedWallets), "Reject infinite stored balance");
        check(loadedWallets.findById(wallet)->getBalance() == wallets.findById(wallet)->getBalance(), "Wallet load failure is non-destructive");
        writeText(categoryPath, "1|Salary|INVALID\n");
        check(!FileManager::loadCategories(categoryPath, loadedCategories) && loadedCategories.getCategories().getSize() == 2, "Category load failure is non-destructive");
        writeText(budgetPath, "1|10|2026-10-31|2026-10-01|0\n");
        check(!FileManager::loadBudgets(budgetPath, loadedBudgets) && loadedBudgets.getBudgets().getSize() == 1, "Budget load failure is non-destructive");

        writeText(transactionPath, "original");
        writeText(transactionPath + ".bak", "backup");
        check(!FileManager::saveTransactions(transactionPath, transactions), "Refuse overwriting unresolved backup");
        check(readText(transactionPath) == "original" && readText(transactionPath + ".bak") == "backup", "Save failure preserves originals");
        fs::path directory = root / "not-a-file";
        fs::create_directory(directory);
        check(!FileManager::saveTransactions(directory.u8string(), transactions) && fs::is_directory(directory), "Reject directory destination");
        check(!FileManager::saveTransactions((root / "missing" / "file.txt").u8string(), transactions), "Report missing parent directory");
        check(!FileManager::ensureDataDirectory(transactionPath), "Report file used as data directory");
        User user("admin", "123456", "");
        User loadedUser;
        std::string userPath = (root / "user.txt").u8string();
        check(FileManager::saveUser(userPath, user) && FileManager::loadUser(userPath, loadedUser) && loadedUser.login("admin", "123456"), "User with empty display name round trip");
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) throw std::runtime_error("Expected test output directory");
        fs::path root = fs::u8path(argv[1]) / std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        testList();
        testDates();
        testManagers();
        testStorage(root);
        std::cout << "PASS: " << checks << " checks (list, dates, transactions, budgets, storage)\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL after " << checks << " checks: " << error.what() << '\n';
        return 1;
    }
}
