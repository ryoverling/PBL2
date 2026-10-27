#include "wallet_dialog.h"
#include "ui/ui_wallet_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

WalletDialog::WalletDialog(const Wallet* wallet, QWidget* parent)
    : QDialog(parent), ui(new Ui::WalletDialog), editing(wallet != nullptr) {
    ui->setupUi(this);
    setWindowTitle(editing ? "Sửa ví" : "Thêm ví");
    nameInput = ui->nameInput;
    typeCombo = ui->typeCombo;
    balanceInput = ui->balanceInput;
    typeCombo->addItem("Tiền mặt", static_cast<int>(WalletType::CASH));
    typeCombo->addItem("Ngân hàng", static_cast<int>(WalletType::BANK));
    typeCombo->addItem("Ví điện tử", static_cast<int>(WalletType::EWALLET));
    typeCombo->addItem("Khác", static_cast<int>(WalletType::OTHER));

    if (wallet != nullptr) {
        nameInput->setText(QString::fromStdString(wallet->getName()));
        int index = typeCombo->findData(static_cast<int>(wallet->getType()));
        if (index >= 0) typeCombo->setCurrentIndex(index);
        balanceInput->setValue(wallet->getBalance());
        balanceInput->setEnabled(false);
        balanceInput->setToolTip("Số dư chỉ thay đổi thông qua giao dịch.");
        ui->balanceLabel->setText("Số dư hiện tại");
    }
    connect(ui->buttons, &QDialogButtonBox::accepted, this, [this]() {
        validateAndAccept();
    });
    connect(ui->buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

WalletDialog::~WalletDialog() {
    delete ui;
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
