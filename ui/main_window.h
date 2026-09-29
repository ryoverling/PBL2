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

    QStackedWidget* pageStack;
    QButtonGroup* navigationGroup;

    DashboardPage* dashboardPage;
    TransactionPage* transactionPage;
    WalletPage* walletPage;
    BudgetPage* budgetPage;
    StatisticsPage* statisticsPage;
    CategoryPage* categoryPage;

    void loadData();
    void initializeDefaults();
    void saveAll();
    void refreshAll();
    void buildUi();
    void handleLogout();

protected:
    void closeEvent(QCloseEvent* event) override;

public:
    MainWindow(QString userName,
               std::function<void()> onLogout,
               QWidget* parent = nullptr);
};
