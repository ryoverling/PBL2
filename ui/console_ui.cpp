#include "console_ui.h"

#include "../storage/file_storage.h"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

#ifdef _WIN32
#include <conio.h>
#include <cstdio>
#include <io.h>
#endif

namespace {

const int MAX_ID = std::numeric_limits<int>::max();
const int PAGE_WIDTH = 90;

std::size_t utf8CharacterSize(const std::string& text, std::size_t position) {
    const unsigned char first = static_cast<unsigned char>(text[position]);
    if (first < 0x80) return 1;

    std::size_t size = 0;
    if (first >= 0xC2 && first <= 0xDF) size = 2;
    else if (first >= 0xE0 && first <= 0xEF) size = 3;
    else if (first >= 0xF0 && first <= 0xF4) size = 4;
    else return 1;

    if (position + size > text.size()) return 1;
    for (std::size_t i = 1; i < size; i++) {
        const unsigned char next = static_cast<unsigned char>(text[position + i]);
        if ((next & 0xC0) != 0x80) return 1;
    }
    return size;
}

std::size_t textWidth(const std::string& text) {
    std::size_t width = 0;
    std::size_t position = 0;
    while (position < text.size()) {
        position += utf8CharacterSize(text, position);
        width++;
    }
    return width;
}

std::string textPrefix(const std::string& text, std::size_t maximumWidth) {
    std::size_t width = 0;
    std::size_t byteCount = 0;
    while (byteCount < text.size() && width < maximumWidth) {
        byteCount += utf8CharacterSize(text, byteCount);
        width++;
    }
    return text.substr(0, byteCount);
}

std::string fixedText(const std::string& text, std::size_t width) {
    const std::size_t currentWidth = textWidth(text);
    if (currentWidth > width) {
        if (width <= 3) return textPrefix(text, width);
        return textPrefix(text, width - 3) + "...";
    }
    return text + std::string(width - currentWidth, ' ');
}

std::string rightText(const std::string& text, std::size_t width) {
    const std::size_t currentWidth = textWidth(text);
    if (currentWidth > width) return fixedText(text, width);
    return std::string(width - currentWidth, ' ') + text;
}

std::string groupedNumber(double value) {
    std::ostringstream output;
    output << std::fixed << std::setprecision(0) << value;
    const std::string raw = output.str();
    const bool negative = !raw.empty() && raw[0] == '-';
    const std::size_t firstDigit = negative ? 1 : 0;

    std::string result = negative ? "-" : "";
    for (std::size_t i = firstDigit; i < raw.size(); i++) {
        const std::size_t digitsRemaining = raw.size() - i;
        if (i > firstDigit && digitsRemaining % 3 == 0) result += '.';
        result += raw[i];
    }
    return result;
}

std::string numberText(double value, std::size_t width) {
    const std::string formatted = groupedNumber(value);
    if (textWidth(formatted) <= width) return formatted;
    return fixedText(formatted, width);
}

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\033[2J\033[H";
#endif
}

void header(const std::string& title) {
    clearScreen();
    std::cout << '+' << std::string(PAGE_WIDTH, '=') << "+\n"
              << '|' << fixedText("  " + title, PAGE_WIDTH) << "|\n"
              << '+' << std::string(PAGE_WIDTH, '=') << "+\n";
}

void footer() {
    std::cout << '+' << std::string(PAGE_WIDTH, '=') << "+\n";
}

void pageLine(const std::string& text) {
    std::string content = "  " + text;
    std::cout << '|' << fixedText(content, PAGE_WIDTH) << "|\n";
}

std::string trim(std::string value) {
    const std::size_t first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    const std::size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string textInput(const std::string& prompt, bool allowEmpty = false) {
    while (true) {
        std::cout << prompt;
        std::string value;
        if (!std::getline(std::cin, value)) throw std::runtime_error("Luong nhap da dong.");
        value = trim(value);
        if (allowEmpty || !value.empty()) return value;
        std::cout << "  [!] Khong duoc de trong.\n";
    }
}

std::string passwordInput(const std::string& prompt) {
#ifdef _WIN32
    if (_isatty(_fileno(stdin))) {
        while (true) {
            std::cout << prompt << std::flush;
            std::string value;

            while (true) {
                const int key = _getch();
                if (key == '\r') {
                    std::cout << '\n';
                    break;
                }
                if (key == 0 || key == 224) {
                    _getch();
                    continue;
                }
                if (key == '\b') {
                    if (!value.empty()) {
                        value.pop_back();
                        std::cout << "\b \b" << std::flush;
                    }
                    continue;
                }
                if (key >= 32 && key <= 126) {
                    value += static_cast<char>(key);
                    std::cout << '*' << std::flush;
                }
            }

            if (!value.empty()) return value;
            std::cout << "  [!] Khong duoc de trong.\n";
        }
    }
#endif
    return textInput(prompt);
}

int intInput(const std::string& prompt, int minimum, int maximum) {
    while (true) {
        std::istringstream input(textInput(prompt));
        int value = 0;
        char extra = 0;
        if ((input >> value) && !(input >> extra) && value >= minimum && value <= maximum) return value;
        std::cout << "  [!] Vui long nhap so tu " << minimum << " den " << maximum << ".\n";
    }
}

int intInputDefault(const std::string& prompt, int minimum, int maximum, int defaultValue) {
    while (true) {
        const std::string text = textInput(prompt, true);
        if (text.empty()) return defaultValue;
        std::istringstream input(text);
        int value = 0;
        char extra = 0;
        if ((input >> value) && !(input >> extra) && value >= minimum && value <= maximum) return value;
        std::cout << "  [!] Vui long nhap so tu " << minimum << " den " << maximum << ".\n";
    }
}

double amountInput(const std::string& prompt, bool allowZero = false) {
    while (true) {
        const std::string text = textInput(prompt);
        bool valid = true;
        bool hasDot = false;
        int groupLength = 0;
        int groupIndex = 0;
        double value = 0;

        for (char character : text) {
            if (character >= '0' && character <= '9') {
                groupLength++;
                value = value * 10 + (character - '0');
                if (!std::isfinite(value)) valid = false;
            } else if (character == '.') {
                hasDot = true;
                if (groupLength == 0 || (groupIndex == 0 && groupLength > 3) ||
                    (groupIndex > 0 && groupLength != 3)) valid = false;
                groupIndex++;
                groupLength = 0;
            } else {
                valid = false;
            }
        }
        if (groupLength == 0 || (hasDot && groupLength != 3)) valid = false;

        if (valid && (allowZero ? value >= 0 : value > 0)) return value;
        std::cout << "  [!] So tien khong hop le. Vi du: 10000 hoac 10.000.\n";
    }
}

std::string dateInput(const std::string& prompt, bool todayAllowed = false) {
    while (true) {
        const std::string value = textInput(prompt, todayAllowed);
        if (value == "0") return "0";
        if (todayAllowed && value.empty()) return MoneyManager::currentDate();
        if (MoneyManager::isValidDate(value)) return value;

        int year = 0;
        int month = 0;
        int day = 0;
        char firstSeparator = 0;
        char secondSeparator = 0;
        char extra = 0;
        std::istringstream input(value);
        if ((input >> year >> firstSeparator >> month >> secondSeparator >> day) &&
            !(input >> extra) && firstSeparator == '-' && secondSeparator == '-') {
            std::ostringstream normalized;
            normalized << std::setw(4) << std::setfill('0') << year << '-'
                       << std::setw(2) << month << '-'
                       << std::setw(2) << day;
            if (MoneyManager::isValidDate(normalized.str())) return normalized.str();
        }

        std::cout << "  [!] Ngay khong hop le. Vi du: 2026-06-06. Nhap 0 de huy.\n";
    }
}

void waitForEnter() {
    std::cout << "\nNhan Enter de tiep tuc...";
    std::string ignored;
    std::getline(std::cin, ignored);
}

std::string moneyText(double value) {
    return groupedNumber(value) + " VND";
}

WalletType inputWalletType() {
    std::cout << "  1. Tien mat   2. Ngan hang   3. Vi dien tu\n";
    return static_cast<WalletType>(intInputDefault("Chon loai vi (Enter = Tien mat): ", 1, 3, 1));
}

void currentMonthRange(std::string& startDate, std::string& endDate) {
    const std::string today = MoneyManager::currentDate();
    const std::string month = today.substr(0, 8);
    startDate = month + "01";

    for (int day = 31; day >= 28; day--) {
        std::ostringstream value;
        value << month << std::setw(2) << std::setfill('0') << day;
        if (MoneyManager::isValidDate(value.str())) {
            endDate = value.str();
            return;
        }
    }
}

} // namespace

int ConsoleUI::run() {
    if (!login()) return 1;

    while (true) {
        header("MONEY MANAGER - QUAN LY TAI CHINH CA NHAN");
        pageLine("Xin chao, " + manager_.getUser().getDisplayName());
        pageLine("");
        pageLine("1. Tong quan tai chinh");
        pageLine("2. Quan ly giao dich");
        pageLine("3. Quan ly vi tien");
        pageLine("4. Quan ly danh muc");
        pageLine("5. Quan ly ngan sach");
        pageLine("6. Bao cao thu chi");
        pageLine("7. Tai khoan");
        pageLine("0. Luu va thoat");
        footer();

        switch (intInput("Lua chon: ", 0, 7)) {
            case 1: dashboard(); break;
            case 2: transactionMenu(); break;
            case 3: walletMenu(); break;
            case 4: categoryMenu(); break;
            case 5: budgetMenu(); break;
            case 6: reportMenu(); break;
            case 7: accountMenu(); break;
            case 0:
                if (save()) {
                    std::cout << "Da luu du lieu. Tam biet!\n";
                } else {
                    std::cout << "Thoat chuong trinh nhung du lieu chua duoc luu.\n";
                }
                return 0;
        }
    }
}

bool ConsoleUI::login() {
    for (int attempt = 1; attempt <= 3; ++attempt) {
        header("DANG NHAP HE THONG");
        pageLine("Nhap thong tin tai khoan de tiep tuc");
        footer();
        const std::string username = textInput("Ten dang nhap: ");
        const std::string password = passwordInput("Mat khau: ");
        if (manager_.login(username, password)) {
            return true;
        }
        std::cout << "Sai thong tin dang nhap. Con " << 3 - attempt << " lan thu.\n";
        if (attempt < 3) waitForEnter();
    }
    return false;
}

void ConsoleUI::dashboard() const {
    header("TONG QUAN TAI CHINH");
    const double income = manager_.getTotalIncome();
    const double expense = manager_.getTotalExpense();
    pageLine("Tong so du hien tai : " + moneyText(manager_.getTotalBalance()));
    pageLine("Tong thu nhap       : " + moneyText(income));
    pageLine("Tong chi tieu       : " + moneyText(expense));
    pageLine("Chenh lech thu - chi: " + moneyText(income - expense));
    pageLine("So giao dich        : " + std::to_string(manager_.getTransactions().getSize()));
    pageLine("");

    std::string startDate;
    std::string endDate;
    currentMonthRange(startDate, endDate);
    pageLine("CAC KHOAN CHI THANG HIEN TAI");

    bool hasCurrentMonthExpense = false;
    for (std::size_t i = 0; i < manager_.getTransactions().getSize(); i++) {
        Transaction* const* transaction = manager_.getTransactions().getAt(i);
        if ((*transaction)->getType() != TransactionType::Expense ||
            (*transaction)->getDate() < startDate || (*transaction)->getDate() > endDate) {
            continue;
        }

        const Category* category = manager_.findCategory((*transaction)->getCategoryId());
        const std::string expenseName = !(*transaction)->getNote().empty()
            ? (*transaction)->getNote()
            : (category != nullptr ? category->getName() : "Khong ro");
        pageLine("- " + expenseName + " - " + moneyText((*transaction)->getAmount()));
        hasCurrentMonthExpense = true;
    }

    if (!hasCurrentMonthExpense) {
        pageLine("Chua co khoan chi nao trong thang nay.");
    }
    footer();
    waitForEnter();
}

void ConsoleUI::printWallets() const {
    std::cout << "+----+-------------------------------+--------------------+--------------------------------+\n"
              << "| ID | Ten vi                        | Loai               | So du                          |\n"
              << "+----+-------------------------------+--------------------+--------------------------------+\n";
    for (std::size_t i = 0; i < manager_.getWallets().getSize(); i++) {
        const Wallet* wallet = manager_.getWallets().getAt(i);
        const std::string balance = numberText(manager_.getWalletBalance(wallet->getId()), 22) + " VND";
        std::cout << "| " << rightText(std::to_string(wallet->getId()), 2) << " | "
                  << fixedText(wallet->getName(), 29) << " | "
                  << fixedText(walletTypeName(wallet->getType()), 18) << " | "
                  << rightText(balance, 30) << " |\n";
    }
    std::cout << "+----+-------------------------------+--------------------+--------------------------------+\n";
}

void ConsoleUI::walletMenu() {
    while (true) {
        header("QUAN LY VI TIEN");
        printWallets();
        pageLine("1. Them vi     2. Sua vi     3. Xoa vi     0. Quay lai");
        footer();
        const int choice = intInput("Lua chon: ", 0, 3);
        if (choice == 0) return;

        if (choice == 1) {
            const std::string name = textInput("Ten vi: ");
            const WalletType type = inputWalletType();
            const double balance = amountInput("So du ban dau: ", true);
            if (manager_.addWallet(name, type, balance) > 0) {
                std::cout << "[OK] Da them vi.\n";
                save();
            }
        } else {
            const int id = intInput("Nhap ID vi (0 = huy): ", 0, MAX_ID);
            if (id == 0) continue;
            if (choice == 2) {
                Wallet* wallet = manager_.findWallet(id);
                if (wallet == nullptr) {
                    std::cout << "[!] Khong tim thay vi.\n";
                } else {
                    std::cout << "  1. Doi ten vi   2. Doi loai vi   3. Dieu chinh so du   0. Huy\n";
                    const int editChoice = intInput("Noi dung can sua: ", 0, 3);
                    if (editChoice == 1) {
                        const std::string name = textInput("Ten vi moi: ");
                        manager_.updateWallet(id, name, wallet->getType());
                        std::cout << "[OK] Da doi ten vi.\n";
                    } else if (editChoice == 2) {
                        const WalletType type = inputWalletType();
                        manager_.updateWallet(id, wallet->getName(), type);
                        std::cout << "[OK] Da doi loai vi.\n";
                    } else if (editChoice == 3) {
                        std::cout << "So du hien tai: " << moneyText(manager_.getWalletBalance(id)) << '\n';
                        const double newBalance = amountInput("Nhap so du thuc te moi: ", true);
                        if (manager_.setWalletBalance(id, newBalance)) {
                            std::cout << "[OK] Da dieu chinh so du vi.\n";
                        }
                    }
                }
            } else {
                std::cout << (manager_.deleteWallet(id)
                                  ? "[OK] Da xoa vi.\n"
                                  : "[!] Khong the xoa vi khong ton tai hoac dang co giao dich.\n");
            }
            save();
        }
        waitForEnter();
    }
}

void ConsoleUI::printCategories() const {
    std::cout << "+----+------------------------------------------------------+------------------------------+\n"
              << "| ID | Ten danh muc                                         | Loai                         |\n"
              << "+----+------------------------------------------------------+------------------------------+\n";
    for (std::size_t i = 0; i < manager_.getCategories().getSize(); i++) {
        const Category* category = manager_.getCategories().getAt(i);
        std::cout << "| " << rightText(std::to_string(category->getId()), 2) << " | "
                  << fixedText(category->getName(), 52) << " | "
                  << fixedText(categoryTypeName(category->getType()), 28) << " |\n";
    }
    std::cout << "+----+------------------------------------------------------+------------------------------+\n";
}

int ConsoleUI::chooseCategory(CategoryType type, bool allowAll) const {
    int count = 0;
    std::cout << "+----+------------------------------------------------------+------------------------------+\n"
              << "| STT| Ten danh muc                                         | Loai                         |\n"
              << "+----+------------------------------------------------------+------------------------------+\n";
    if (allowAll) {
        std::cout << "| " << rightText("0", 2) << " | "
                  << fixedText("Tat ca danh muc", 52) << " | "
                  << fixedText("Chi tieu", 28) << " |\n";
    }
    for (std::size_t i = 0; i < manager_.getCategories().getSize(); i++) {
        const Category* category = manager_.getCategories().getAt(i);
        if (category->getType() != type) continue;
        count++;
        std::cout << "| " << rightText(std::to_string(count), 2) << " | "
                  << fixedText(category->getName(), 52) << " | "
                  << fixedText(categoryTypeName(category->getType()), 28) << " |\n";
    }
    std::cout << "+----+------------------------------------------------------+------------------------------+\n";

    const int choice = allowAll
        ? intInput("Chon STT danh muc (0 = Tat ca): ", 0, count)
        : intInput("Chon STT danh muc (0 = huy): ", 0, count);
    if (choice == 0) return allowAll ? 0 : -1;

    int position = 0;
    for (std::size_t i = 0; i < manager_.getCategories().getSize(); i++) {
        const Category* category = manager_.getCategories().getAt(i);
        if (category->getType() != type) continue;
        position++;
        if (position == choice) return category->getId();
    }
    return -1;
}

void ConsoleUI::categoryMenu() {
    while (true) {
        header("QUAN LY DANH MUC");
        printCategories();
        pageLine("1. Them danh muc     2. Doi ten     3. Xoa     0. Quay lai");
        footer();
        const int choice = intInput("Lua chon: ", 0, 3);
        if (choice == 0) return;

        if (choice == 1) {
            const std::string name = textInput("Ten danh muc: ");
            const CategoryType type = static_cast<CategoryType>(
                intInput("Loai (1: Thu nhap, 2: Chi tieu): ", 1, 2));
            std::cout << (manager_.addCategory(name, type) > 0
                              ? "[OK] Da them danh muc.\n" : "[!] Du lieu khong hop le.\n");
        } else {
            const int id = intInput("Nhap ID danh muc (0 = huy): ", 0, MAX_ID);
            if (id == 0) continue;
            if (choice == 2) {
                const std::string name = textInput("Ten danh muc moi: ");
                std::cout << (manager_.renameCategory(id, name)
                                  ? "[OK] Da doi ten.\n" : "[!] Khong tim thay danh muc.\n");
            } else {
                std::cout << (manager_.deleteCategory(id)
                                  ? "[OK] Da xoa danh muc.\n"
                                  : "[!] Danh muc khong ton tai hoac dang duoc su dung.\n");
            }
        }
        save();
        waitForEnter();
    }
}

void ConsoleUI::printTransactions() const {
    std::cout << "+----+------------+-----------+--------------+--------------+--------------+---------------+\n"
              << "| ID | Ngay       | Loai      | So tien      | Vi           | Danh muc     | Ghi chu       |\n"
              << "+----+------------+-----------+--------------+--------------+--------------+---------------+\n";
    for (std::size_t i = 0; i < manager_.getTransactions().getSize(); i++) {
        Transaction* const* transaction = manager_.getTransactions().getAt(i);
        const Wallet* wallet = manager_.findWallet((*transaction)->getWalletId());
        const Category* category = manager_.findCategory((*transaction)->getCategoryId());
        std::cout << "| " << rightText(std::to_string((*transaction)->getId()), 2) << " | "
                  << fixedText((*transaction)->getDate(), 10) << " | "
                  << fixedText((*transaction)->getTypeName(), 9) << " | "
                  << rightText(numberText((*transaction)->getAmount(), 12), 12) << " | "
                  << fixedText(wallet ? wallet->getName() : "?", 12) << " | "
                  << fixedText(category ? category->getName() : "?", 12) << " | "
                  << fixedText((*transaction)->getNote(), 13) << " |\n";
    }
    std::cout << "+----+------------+-----------+--------------+--------------+--------------+---------------+\n";
}

Transaction* ConsoleUI::inputTransaction(int id) const {
    if (manager_.getWallets().isEmpty()) {
        std::cout << "[!] Can tao vi truoc.\n";
        return nullptr;
    }

    const TransactionType type = static_cast<TransactionType>(
        intInput("Loai (1: Thu nhap, 2: Chi tieu): ", 1, 2));
    const CategoryType categoryType = type == TransactionType::Income
                                          ? CategoryType::Income : CategoryType::Expense;
    bool hasCategory = false;
    for (std::size_t i = 0; i < manager_.getCategories().getSize(); i++) {
        const Category* category = manager_.getCategories().getAt(i);
        if (category->getType() == categoryType) hasCategory = true;
    }
    if (!hasCategory) {
        std::cout << "[!] Can tao danh muc phu hop truoc.\n";
        return nullptr;
    }

    int walletId = 0;
    if (manager_.getWallets().getSize() == 1) {
        const Wallet* wallet = manager_.getWallets().getAt(0);
        walletId = wallet->getId();
        std::cout << "Vi duoc chon tu dong: " << wallet->getName() << '\n';
    } else {
        printWallets();
        while (manager_.findWallet(walletId) == nullptr) {
            walletId = intInput("ID vi: ", 1, MAX_ID);
            if (manager_.findWallet(walletId) == nullptr) std::cout << "[!] ID vi khong ton tai.\n";
        }
    }

    int categoryId = 0;
    int matchingCategories = 0;
    for (std::size_t i = 0; i < manager_.getCategories().getSize(); i++) {
        const Category* category = manager_.getCategories().getAt(i);
        if (category->getType() == categoryType) {
            matchingCategories++;
            categoryId = category->getId();
        }
    }
    if (matchingCategories == 1) {
        std::cout << "Danh muc duoc chon tu dong: "
                  << manager_.findCategory(categoryId)->getName() << '\n';
    } else {
        categoryId = chooseCategory(categoryType, false);
        if (categoryId == -1) return nullptr;
    }

    const double amount = amountInput("So tien: ");
    const std::string date = dateInput("Ngay (YYYY-MM-DD, Enter = hom nay, 0 = huy): ", true);
    if (date == "0") return nullptr;
    const std::string note = textInput("Ghi chu (co the de trong): ", true);
    return type == TransactionType::Income
        ? static_cast<Transaction*>(new Income(id, amount, date, note, walletId, categoryId))
        : static_cast<Transaction*>(new Expense(id, amount, date, note, walletId, categoryId));
}

void ConsoleUI::transactionMenu() {
    while (true) {
        header("QUAN LY GIAO DICH");
        printTransactions();
        pageLine("1. Them giao dich     0. Quay lai");
        footer();
        const int choice = intInput("Lua chon: ", 0, 1);
        if (choice == 0) return;

        Transaction* transaction = inputTransaction(1);
        if (transaction != nullptr) {
            const int id = manager_.addTransaction(transaction->getType(), transaction->getAmount(),
                transaction->getDate(), transaction->getNote(), transaction->getWalletId(),
                transaction->getCategoryId());
            delete transaction;
            std::cout << (id > 0 ? "[OK] Da them giao dich.\n" : "[!] Du lieu khong hop le.\n");
        }
        save();
        waitForEnter();
    }
}

void ConsoleUI::printBudgets() const {
    std::cout << "+----+------------+------------+------------------------+----------------+-----------------+\n"
              << "| ID | Bat dau    | Ket thuc   | Danh muc               | Da chi         | Gioi han        |\n"
              << "+----+------------+------------+------------------------+----------------+-----------------+\n";
    for (std::size_t i = 0; i < manager_.getBudgets().getSize(); i++) {
        const Budget* budget = manager_.getBudgets().getAt(i);
        const Category* category = manager_.findCategory(budget->getCategoryId());
        std::cout << "| " << rightText(std::to_string(budget->getId()), 2) << " | "
                  << fixedText(budget->getStartDate(), 10) << " | "
                  << fixedText(budget->getEndDate(), 10) << " | "
                  << fixedText(category ? category->getName() : "Tat ca", 22) << " | "
                  << rightText(numberText(manager_.getBudgetSpent(*budget), 14), 14) << " | "
                  << rightText(numberText(budget->getLimit(), 15), 15) << " |\n";
    }
    std::cout << "+----+------------+------------+------------------------+----------------+-----------------+\n";
}

bool ConsoleUI::inputBudget(double& limit, std::string& startDate,
                            std::string& endDate, int& categoryId) const {
    limit = amountInput("Gioi han chi tieu: ");
    std::cout << "  1. Thang hien tai   2. Khoang ngay tuy chon   0. Huy\n";
    const int periodChoice = intInput("Chon thoi gian: ", 0, 2);
    if (periodChoice == 0) return false;
    if (periodChoice == 1) {
        currentMonthRange(startDate, endDate);
        std::cout << "Ap dung tu " << startDate << " den " << endDate << ".\n";
    } else {
        startDate = dateInput("Ngay bat dau (YYYY-MM-DD, 0 = huy): ");
        if (startDate == "0") return false;
        endDate = dateInput("Ngay ket thuc (YYYY-MM-DD, 0 = huy): ");
        if (endDate == "0") return false;
        if (startDate > endDate) {
            std::cout << "[!] Ngay bat dau phai truoc ngay ket thuc.\n";
            return false;
        }
    }
    categoryId = chooseCategory(CategoryType::Expense, true);
    return true;
}

void ConsoleUI::budgetMenu() {
    while (true) {
        header("QUAN LY NGAN SACH");
        printBudgets();
        pageLine("1. Them ngan sach     2. Dieu chinh     3. Xoa     0. Quay lai");
        footer();
        const int choice = intInput("Lua chon: ", 0, 3);
        if (choice == 0) return;

        double limit = 0;
        int categoryId = 0;
        std::string startDate, endDate;
        if (choice == 1) {
            if (inputBudget(limit, startDate, endDate, categoryId)) {
                std::cout << (manager_.addBudget(limit, startDate, endDate, categoryId) > 0
                                  ? "[OK] Da them ngan sach.\n" : "[!] Du lieu khong hop le.\n");
            }
        } else {
            const int id = intInput("Nhap ID ngan sach (0 = huy): ", 0, MAX_ID);
            if (id == 0) continue;
            if (choice == 2) {
                const Budget* oldBudget = manager_.findBudget(id);
                if (oldBudget == nullptr) {
                    std::cout << "[!] Khong tim thay ngan sach.\n";
                } else {
                    std::cout << "  1. Chi doi gioi han   2. Sua toan bo   0. Huy\n";
                    const int editChoice = intInput("Noi dung can sua: ", 0, 2);
                    if (editChoice == 1) {
                        limit = amountInput("Gioi han moi: ");
                        manager_.updateBudget(id, limit, oldBudget->getStartDate(),
                            oldBudget->getEndDate(), oldBudget->getCategoryId());
                        std::cout << "[OK] Da doi gioi han.\n";
                    } else if (editChoice == 2 && inputBudget(limit, startDate, endDate, categoryId)) {
                        manager_.updateBudget(id, limit, startDate, endDate, categoryId);
                        std::cout << "[OK] Da dieu chinh ngan sach.\n";
                    }
                }
            } else {
                std::cout << (manager_.deleteBudget(id)
                                  ? "[OK] Da xoa ngan sach.\n" : "[!] Khong tim thay ngan sach.\n");
            }
        }
        save();
        waitForEnter();
    }
}

void ConsoleUI::reportMenu() const {
    header("BAO CAO THU CHI THEO THOI GIAN");
    std::cout << "  1. Thang hien tai   2. Khoang ngay tuy chon   0. Quay lai\n";
    const int periodChoice = intInput("Lua chon: ", 0, 2);
    if (periodChoice == 0) return;

    std::string startDate;
    std::string endDate;
    if (periodChoice == 1) {
        currentMonthRange(startDate, endDate);
    } else {
        startDate = dateInput("Tu ngay (YYYY-MM-DD, 0 = huy): ");
        if (startDate == "0") return;
        endDate = dateInput("Den ngay (YYYY-MM-DD, 0 = huy): ");
        if (endDate == "0") return;
    }
    if (startDate > endDate) {
        std::cout << "[!] Khoang ngay khong hop le.\n";
        waitForEnter();
        return;
    }

    const double income = manager_.getTotalIncome(startDate, endDate);
    const double expense = manager_.getTotalExpense(startDate, endDate);
    pageLine("Khoang thoi gian: " + startDate + " den " + endDate);
    pageLine("Tong thu : " + moneyText(income));
    pageLine("Tong chi : " + moneyText(expense));
    pageLine("Chenh lech: " + moneyText(income - expense));
    pageLine("");
    pageLine("CHI TIET CHI TIEU THEO DANH MUC");
    for (std::size_t i = 0; i < manager_.getCategories().getSize(); i++) {
        const Category* category = manager_.getCategories().getAt(i);
        if (category->getType() != CategoryType::Expense) continue;
        const double total = manager_.getCategoryExpense(category->getId(), startDate, endDate);
        if (total > 0) {
            pageLine("- " + category->getName() + ": " + moneyText(total));
        }
    }
    footer();
    waitForEnter();
}

void ConsoleUI::accountMenu() {
    header("THONG TIN TAI KHOAN");
    std::cout << "  Ten dang nhap: " << manager_.getUser().getUsername()
              << "\n  Ten hien thi : " << manager_.getUser().getDisplayName() << "\n\n"
              << "  1. Doi ten hien thi     2. Doi mat khau     0. Quay lai\n";
    const int choice = intInput("Lua chon: ", 0, 2);
    if (choice == 0) return;
    if (choice == 1) {
        const std::string name = textInput("Ten hien thi moi: ");
        std::cout << (manager_.changeDisplayName(name)
                          ? "[OK] Da cap nhat.\n" : "[!] Ten khong hop le.\n");
    } else {
        const std::string oldPassword = passwordInput("Mat khau hien tai: ");
        const std::string newPassword = passwordInput("Mat khau moi: ");
        std::cout << (manager_.changePassword(oldPassword, newPassword)
                          ? "[OK] Da doi mat khau.\n"
                          : "[!] Mat khau hien tai khong dung hoac mat khau moi khong hop le.\n");
    }
    save();
    waitForEnter();
}

bool ConsoleUI::save() const {
    if (!FileStorage::save(manager_)) {
        std::cerr << "[!] Khong the luu cac file .dat trong thu muc data.\n";
        return false;
    }
    return true;
}
