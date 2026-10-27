#pragma once

#include <QDialog>
#include "../model/wallet.h"

class QLineEdit;
class QComboBox;
class QDoubleSpinBox;
namespace Ui { class WalletDialog; }

class WalletDialog : public QDialog {
private:
    Ui::WalletDialog* ui;
    QLineEdit* nameInput;
    QComboBox* typeCombo;
    QDoubleSpinBox* balanceInput;
    bool editing;

    void validateAndAccept();

public:
    explicit WalletDialog(const Wallet* wallet = nullptr, QWidget* parent = nullptr);
    ~WalletDialog();

    QString walletName() const;
    WalletType walletType() const;
    double initialBalance() const;
};
