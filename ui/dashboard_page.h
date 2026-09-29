#pragma once

#include <QWidget>

class QLabel;
class QProgressBar;
class QTableWidget;
class DonutChartWidget;

class TransactionManager;
class WalletManager;
class CategoryManager;
class BudgetManager;

class DashboardPage : public QWidget {
private:
    TransactionManager* transactions;
    WalletManager* wallets;
    CategoryManager* categories;
    BudgetManager* budgets;

    QLabel* totalBalanceLabel;
    QLabel* incomeLabel;
    QLabel* expenseLabel;
    QLabel* differenceLabel;
    QLabel* budgetDetailLabel;
    QProgressBar* budgetProgress;
    QTableWidget* recentTable;
    DonutChartWidget* donutChart;

public:
    DashboardPage(TransactionManager* transactions,
                  WalletManager* wallets,
                  CategoryManager* categories,
                  BudgetManager* budgets,
                  QWidget* parent = nullptr);

    void refresh();
};
