#pragma once

#include <QDialog>
#include "../model/wallet.h"

class QLineEdit;
class QComboBox;
class QDoubleSpinBox;

class WalletDialog : public QDialog {
private:
    QLineEdit* nameInput;
    QComboBox* typeCombo;
    QDoubleSpinBox* balanceInput;
    bool editing;

    void validateAndAccept();

public:
    explicit WalletDialog(const Wallet* wallet = nullptr, QWidget* parent = nullptr);

    QString walletName() const;
    WalletType walletType() const;
    double initialBalance() const;
};
