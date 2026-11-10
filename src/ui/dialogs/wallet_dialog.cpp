#include "wallet_dialog.h"
#include "ui_wallet_dialog.h"
#include "../gui_helpers.h"
#include "../../core/app/money_manager.h"

WalletDialog::WalletDialog(MoneyManager& mm, int id, QWidget* parent)
    : QDialog(parent), ui(new Ui::WalletDialog), moneyManager(mm), walletId(id),
      walletType(WalletType::CASH), balance(0) {
    ui->setupUi(this);
    UiHelper::setupWindowTheme(this);

    ui->cboType->addItem("Tiền mặt", static_cast<int>(WalletType::CASH));
    ui->cboType->addItem("Tài khoản ngân hàng", static_cast<int>(WalletType::BANK));
    ui->cboType->addItem("Ví điện tử", static_cast<int>(WalletType::EWALLET));
    setupMode();
    on_cboType_currentIndexChanged(ui->cboType->currentIndex());
}

WalletDialog::~WalletDialog() {
    delete ui;
}

void WalletDialog::setupMode() {
    if (walletId == -1) {
        ui->lblDialogTitle->setText("Thêm ví tiền mới");
        ui->edtBalance->setText("0");
    } else {
        setupExistingWallet();
    }
}

void WalletDialog::setupExistingWallet() {
    const Wallet* wallet = moneyManager.getWalletManager().findById(walletId);
    if (!wallet) return;
    ui->edtName->setText(QString::fromStdString(wallet->getName()));
    ui->cboType->setCurrentIndex(ui->cboType->findData(static_cast<int>(wallet->getType())));
    ui->edtBalance->setText(QString::number(wallet->getBalance()));
    ui->lblDialogTitle->setText("Sửa thông tin ví");
    ui->lblBalance->hide();
    ui->edtBalance->hide();
}

void WalletDialog::on_btnSave_clicked() {
    walletName = ui->edtName->text().trimmed().toStdString();
    walletType = static_cast<WalletType>(ui->cboType->currentData().toInt());
    bool valid = false;
    balance = ui->edtBalance->text().trimmed().toLongLong(&valid);

    if (!UiHelper::isValidName(QString::fromStdString(walletName))) {
        UiHelper::showError(this, "Tên ví không hợp lệ.");
        return;
    }
    if (walletId == -1) {
        if (!valid) {
            UiHelper::showError(this, "Số dư ban đầu phải là số nguyên.");
            return;
        }
        if (balance < 0) {
            UiHelper::showError(this, "Số dư ban đầu của ví không được nhỏ hơn 0.");
            return;
        }
    }
    accept();
}

void WalletDialog::on_btnCancel_clicked() {
    reject();
}

void WalletDialog::on_cboType_currentIndexChanged(int) {
    WalletType type = static_cast<WalletType>(ui->cboType->currentData().toInt());
    QString kind = "cash";
    if (type == WalletType::BANK) kind = "bank";
    else if (type == WalletType::EWALLET) kind = "ewallet";
    ui->badgeTypePreview->setProperty("kind", kind);
    ui->badgeTypePreview->update();
}
