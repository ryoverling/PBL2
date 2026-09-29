#pragma once

#include <QWidget>
#include <functional>

class QComboBox;
class QDateEdit;
class QLineEdit;
class QTableWidget;

class TransactionManager;
class WalletManager;
class CategoryManager;

class TransactionPage : public QWidget {
private:
    TransactionManager* transactions;
    WalletManager* wallets;
    CategoryManager* categories;
    std::function<void()> onChanged;

    QLineEdit* searchInput;
    QComboBox* typeFilter;
    QComboBox* walletFilter;
    QComboBox* categoryFilter;
    QComboBox* periodFilter;
    QDateEdit* fromDate;
    QDateEdit* toDate;
    QTableWidget* table;

    int selectedId() const;
    void rebuildFilters();
    void updatePeriodControls();
    bool passesDateFilter(const std::string& dateTime) const;

    void addTransaction();
    void editTransaction();
    void deleteTransaction();

public:
    TransactionPage(TransactionManager* transactions,
                    WalletManager* wallets,
                    CategoryManager* categories,
                    std::function<void()> onChanged,
                    QWidget* parent = nullptr);

    void refresh();
};
