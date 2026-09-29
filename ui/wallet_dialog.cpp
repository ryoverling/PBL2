#include "wallet_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

WalletDialog::WalletDialog(const Wallet* wallet, QWidget* parent)
    : QDialog(parent), editing(wallet != nullptr) {

    setWindowTitle(editing ? "Sửa ví" : "Thêm ví");
    setMinimumWidth(400);

    nameInput = new QLineEdit();
    nameInput->setPlaceholderText("Ví dụ: Tiền mặt");

    typeCombo = new QComboBox();
    typeCombo->addItem("Tiền mặt", static_cast<int>(WalletType::CASH));
    typeCombo->addItem("Ngân hàng", static_cast<int>(WalletType::BANK));
    typeCombo->addItem("Ví điện tử", static_cast<int>(WalletType::EWALLET));
    typeCombo->addItem("Khác", static_cast<int>(WalletType::OTHER));

    balanceInput = new QDoubleSpinBox();
    balanceInput->setRange(-1000000000000.0, 1000000000000.0);
    balanceInput->setDecimals(0);
    balanceInput->setSingleStep(50000);
    balanceInput->setSuffix(" đ");

    if (wallet != nullptr) {
        nameInput->setText(QString::fromStdString(wallet->getName()));
        int index = typeCombo->findData(static_cast<int>(wallet->getType()));
        if (index >= 0) typeCombo->setCurrentIndex(index);
        balanceInput->setValue(wallet->getBalance());
        balanceInput->setEnabled(false);
        balanceInput->setToolTip("Số dư chỉ thay đổi thông qua giao dịch.");
    }

    QFormLayout* form = new QFormLayout();
    form->addRow("Tên ví", nameInput);
    form->addRow("Loại ví", typeCombo);
    form->addRow(editing ? "Số dư hiện tại" : "Số dư ban đầu", balanceInput);

    QDialogButtonBox* buttons = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel
    );

    QVBoxLayout* root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addSpacing(10);
    root->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        validateAndAccept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void WalletDialog::validateAndAccept() {
    if (nameInput->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Thiếu dữ liệu", "Vui lòng nhập tên ví.");
        return;
    }
    accept();
}

QString WalletDialog::walletName() const {
    return nameInput->text().trimmed();
}

WalletType WalletDialog::walletType() const {
    return static_cast<WalletType>(typeCombo->currentData().toInt());
}

double WalletDialog::initialBalance() const {
    return balanceInput->value();
}
