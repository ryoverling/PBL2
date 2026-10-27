#include "ui/main_window.h"
#include "ui/login_window.h"
#include "ui/transaction_page.h"
#include "ui/transaction_dialog.h"
#include "ui/budget_page.h"
#include "ui/budget_dialog.h"
#include "ui/data_paths.h"
#include "storage/file_manager.h"

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTimer>
#include <cstdlib>
#include <iostream>

namespace {
    int checks = 0;
    void check(bool condition, const char* message) {
        ++checks;
        if (!condition) {
            std::cerr << "FAIL: " << message << '\n';
            std::exit(1);
        }
    }

    template <typename T>
    T* page(QWidget& parent) {
        for (QWidget* widget : parent.findChildren<QWidget*>()) {
            if (auto result = dynamic_cast<T*>(widget)) return result;
        }
        check(false, "Find page");
        return nullptr;
    }

    void click(QWidget& parent, const QString& label) {
        for (QPushButton* button : parent.findChildren<QPushButton*>()) {
            if (button->text() == label) { button->click(); return; }
        }
        check(false, "Find action button");
    }

    void writeFile(const std::string& path, const QByteArray& text) {
        QFile file(QString::fromStdString(path));
        check(file.open(QIODevice::WriteOnly | QIODevice::Truncate), "Open UI fixture");
        check(file.write(text) == text.size(), "Write UI fixture");
    }

    QByteArray readFile(const std::string& path) {
        QFile file(QString::fromStdString(path));
        check(file.open(QIODevice::ReadOnly), "Read UI fixture");
        return file.readAll();
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    // CMake places this executable in its own ui-tests folder; it never uses
    // the application's real data folder beside MoneyManager.exe.
    check(QCoreApplication::applicationDirPath().contains("ui-tests"), "Isolated UI test location");
    check(FileManager::ensureDataDirectory(DataPaths::dataDir().toStdString()), "Create UI fixture directory");
    check(!QFile::exists(QString::fromStdString(DataPaths::transactionFile() + ".bak")), "No unresolved UI test backup");
    CategoryManager categories;
    WalletManager wallets;
    TransactionManager transactions;
    BudgetManager budgets;
    categories.createCategory("Salary", CategoryType::INCOME);
    categories.createCategory("Food", CategoryType::EXPENSE);
    wallets.createWallet("Cash", WalletType::CASH, 1000);
    check(FileManager::saveCategories(DataPaths::categoryFile(), categories) &&
          FileManager::saveWallets(DataPaths::walletFile(), wallets) &&
          FileManager::saveTransactions(DataPaths::transactionFile(), transactions) &&
          FileManager::saveBudgets(DataPaths::budgetFile(), budgets), "Seed UI fixture");

    double nextAmount = 1;
    TransactionType nextType = TransactionType::INCOME;
    int warnings = 0;
    QTimer modalHandler;
    QObject::connect(&modalHandler, &QTimer::timeout, [&]() {
        QWidget* modal = QApplication::activeModalWidget();
        if (auto message = qobject_cast<QMessageBox*>(modal)) {
            if (message->icon() != QMessageBox::Question) ++warnings;
            QMessageBox::StandardButton answer = message->standardButtons().testFlag(QMessageBox::Yes)
                ? QMessageBox::Yes : QMessageBox::Ok;
            message->button(answer)->click();
        } else if (auto dialog = dynamic_cast<TransactionDialog*>(modal)) {
            for (QComboBox* combo : dialog->findChildren<QComboBox*>()) {
                if (combo->findText("Thu nhập") >= 0 && combo->findText("Chi tiêu") >= 0) {
                    combo->setCurrentIndex(combo->findData(static_cast<int>(nextType)));
                }
            }
            dialog->findChild<QDoubleSpinBox*>()->setValue(nextAmount);
            dialog->findChild<QDialogButtonBox*>()->button(QDialogButtonBox::Save)->click();
        } else if (auto dialog = dynamic_cast<BudgetDialog*>(modal)) {
            dialog->findChild<QDoubleSpinBox*>()->setValue(nextAmount);
            dialog->findChild<QDialogButtonBox*>()->button(QDialogButtonBox::Save)->click();
        }
    });
    modalHandler.start(10);

    bool loggedOut = false;
    MainWindow window("Test", [&]() { loggedOut = true; });
    check(window.isReady(), "Main window loads data");
    window.show();
    check(window.findChild<QStackedWidget*>()->count() == 6, "Six desktop pages constructed");
    auto transactionPage = page<TransactionPage>(window);
    auto table = transactionPage->findChild<QTableWidget*>();
    nextAmount = 12345678;
    click(*transactionPage, "+ Thêm giao dịch");
    check(table->rowCount() == 1, "Add transaction through dialog");
    TransactionManager loadedTransactions;
    check(FileManager::loadTransactions(DataPaths::transactionFile(), loadedTransactions) &&
          loadedTransactions.findById(1)->getAmount() == 12345678 && loadedTransactions.findById(1)->getNote().empty(),
          "UI saves exact amount and empty note");
    table->selectRow(0);
    nextType = TransactionType::EXPENSE;
    nextAmount = 200;
    click(*transactionPage, "Sửa");
    WalletManager loadedWallets;
    check(FileManager::loadWallets(DataPaths::walletFile(), loadedWallets) &&
          loadedWallets.findById(1)->getBalance() == 800, "UI edit changes type and reverses old balance effect");

    auto budgetPage = page<BudgetPage>(window);
    nextAmount = 100;
    click(*budgetPage, "+ Tạo ngân sách");
    auto budgetTable = budgetPage->findChild<QTableWidget*>();
    check(budgetTable->rowCount() == 1 && budgetTable->item(0, 6)->text() == "Vượt ngân sách", "Budget page uses current expenses");
    budgetTable->selectRow(0);
    nextAmount = 300;
    click(*budgetPage, "Sửa");
    check(budgetTable->item(0, 6)->text() == "Trong giới hạn", "Budget update refreshes status");
    check(warnings == 0, "Normal actions produce no warnings");

    writeFile(DataPaths::transactionFile() + ".bak", "test backup");
    nextType = TransactionType::INCOME;
    nextAmount = 5;
    click(*transactionPage, "+ Thêm giao dịch");
    check(warnings == 1, "Failed save is reported");
    check(!window.close() && window.isVisible() && warnings == 2, "Failed save prevents closing with unsaved data");
    check(QFile::remove(QString::fromStdString(DataPaths::transactionFile() + ".bak")), "Remove only simulated backup");
    click(window, "Đăng xuất");
    check(loggedOut && !window.isVisible(), "Logout saves pending changes then returns to login");

    MainWindow reopened("Test", []() {});
    check(reopened.isReady(), "Reopen saved session");
    auto reopenedPage = page<TransactionPage>(reopened);
    auto reopenedTable = reopenedPage->findChild<QTableWidget*>();
    check(reopenedTable->rowCount() == 2, "Both transactions survive reopen");
    int expenseRow = -1;
    for (int row = 0; row < reopenedTable->rowCount(); ++row) {
        if (reopenedTable->item(row, 0)->data(Qt::UserRole).toInt() == 1) expenseRow = row;
    }
    check(expenseRow >= 0, "Find saved expense");
    reopenedTable->selectRow(expenseRow);
    click(*reopenedPage, "Xóa");
    check(reopenedTable->rowCount() == 1 && FileManager::loadWallets(DataPaths::walletFile(), loadedWallets) &&
          loadedWallets.findById(1)->getBalance() == 1005, "UI delete restores balance and persists");
    check(reopened.close(), "Close clean session");

    writeFile(DataPaths::transactionFile(), "broken record\n");
    int before = warnings;
    MainWindow damaged("Test", []() {});
    check(!damaged.isReady() && warnings == before + 1, "Invalid startup data is reported");
    check(readFile(DataPaths::transactionFile()) == "broken record\n", "Startup does not overwrite invalid data");
    writeFile(DataPaths::userFile(), "broken account\n");
    LoginWindow login;
    check(readFile(DataPaths::userFile()) == "broken account\n", "Login does not replace corrupt account with default");
    std::cout << "PASS: " << checks << " UI checks (dialogs, save failure, logout, reload, corrupt data)\n";
    return 0;
}
