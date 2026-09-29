#pragma once

#include <QWidget>
#include <functional>

class QLabel;
class QTableWidget;

class WalletManager;
class TransactionManager;

class WalletPage : public QWidget {
private:
    WalletManager* wallets;
    TransactionManager* transactions;
    std::function<void()> onChanged;

    QLabel* totalLabel;
    QTableWidget* table;

    int selectedId() const;
    void addWallet();
    void editWallet();
    void deleteWallet();

public:
    WalletPage(WalletManager* wallets,
               TransactionManager* transactions,
               std::function<void()> onChanged,
               QWidget* parent = nullptr);

    void refresh();
};
