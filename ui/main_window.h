#pragma once

#include <QMainWindow>
#include <functional>

#include "../manager/category_manager.h"
#include "../manager/wallet_manager.h"
#include "../manager/transaction_manager.h"
#include "../manager/budget_manager.h"

class QStackedWidget;
class QButtonGroup;

class DashboardPage;
class TransactionPage;
class WalletPage;
class BudgetPage;
class StatisticsPage;
class CategoryPage;

class MainWindow : public QMainWindow {
private:
    QString userName;
    std::function<void()> onLogout;

    CategoryManager categories;
    WalletManager wallets;
    TransactionManager transactions;
    BudgetManager budgets;
    bool dataReady = false;
    bool unsavedChanges = false;
    bool loggingOut = false;

    QStackedWidget* pageStack = nullptr;
    QButtonGroup* navigationGroup = nullptr;

    DashboardPage* dashboardPage = nullptr;
    TransactionPage* transactionPage = nullptr;
    WalletPage* walletPage = nullptr;
    BudgetPage* budgetPage = nullptr;
    StatisticsPage* statisticsPage = nullptr;
    CategoryPage* categoryPage = nullptr;

    bool loadData();
    void initializeDefaults();
    bool saveAll();
    void refreshAll();
    void buildUi();
    void handleLogout();

protected:
    void closeEvent(QCloseEvent* event) override;

public:
    bool isReady() const { return dataReady; }
    MainWindow(QString userName,
               std::function<void()> onLogout,
               QWidget* parent = nullptr);
};
