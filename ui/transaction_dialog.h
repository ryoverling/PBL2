#pragma once

#include <QDialog>

class QComboBox;
class QDoubleSpinBox;
class QTextEdit;
class QLabel;

class Transaction;
class WalletManager;
class CategoryManager;

class TransactionDialog : public QDialog {
private:
    WalletManager* wallets;
    CategoryManager* categories;
    const Transaction* existing;

    QComboBox* typeCombo;
    QDoubleSpinBox* amountInput;
    QComboBox* categoryCombo;
    QComboBox* walletCombo;
    QTextEdit* noteInput;
    QLabel* timeLabel;

    void populateWallets();
    void populateCategories();
    void validateAndAccept();

public:
    TransactionDialog(WalletManager* wallets,
                      CategoryManager* categories,
                      const Transaction* existing = nullptr,
                      QWidget* parent = nullptr);

    Transaction* createTransaction(int id) const;
};
