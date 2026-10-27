#pragma once

#include <QWidget>
#include <functional>

class QLabel;
class QTableWidget;

class WalletManager;
class TransactionManager;
namespace Ui { class WalletPage; }

class WalletPage : public QWidget {
private:
    Ui::WalletPage* ui;
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
    ~WalletPage();

    void refresh();
};
