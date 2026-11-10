#pragma once

#include <QDialog>
#include <string>
#include "../../core/model/wallet.h"

class MoneyManager;

namespace Ui {
class WalletDialog;
}

// Dialog dùng chung cho Thêm ví và Sửa ví
class WalletDialog : public QDialog {
    Q_OBJECT

private:
    Ui::WalletDialog* ui;
    MoneyManager& moneyManager;
    int walletId;

    std::string walletName;
    WalletType walletType;
    long long balance;

    void setupMode();
    void setupExistingWallet();

private slots:
    void on_btnSave_clicked();
    void on_btnCancel_clicked();
    void on_cboType_currentIndexChanged(int index);

public:
    explicit WalletDialog(MoneyManager& mm, int id = -1, QWidget* parent = nullptr);
    ~WalletDialog();

    std::string getName() const { return walletName; }
    WalletType getType() const { return walletType; }
    long long getBalance() const { return balance; }
};

