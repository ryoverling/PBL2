#include "wallet_page.h"
#include "../gui_helpers.h"
#include "../dialogs/wallet_dialog.h"
#include "../../core/app/money_manager.h"

void WalletPage::on_btnAdd_clicked() {
    WalletDialog dialog(moneyManager, -1, this);
    if (dialog.exec() != QDialog::Accepted) return;
    if (moneyManager.addWallet(dialog.getName(), dialog.getType(), dialog.getBalance())) saveAndRefresh();
    else UiHelper::showError(this, "Không thể thêm ví. Kiểm tra lại tên ví.");
}

void WalletPage::on_btnEdit_clicked() {
    int id = getSelectedWalletId();
    if (id < 0) return;
    WalletDialog dialog(moneyManager, id, this);
    if (dialog.exec() != QDialog::Accepted) return;
    if (moneyManager.updateWallet(id, dialog.getName(), dialog.getType())) saveAndRefresh();
    else UiHelper::showError(this, "Không thể sửa ví. Kiểm tra lại tên ví.");
}

void WalletPage::on_btnDelete_clicked() {
    int id = getSelectedWalletId();
    if (id < 0) return;
    const Wallet* wallet = moneyManager.getWalletManager().findById(id);
    if (!wallet) return;
    QString name = QString::fromStdString(wallet->getName());
    if (!UiHelper::askConfirmation(this, "Xác nhận xóa ví", "Bạn có chắc muốn xóa ví \"" + name + "\"?")) return;
    if (moneyManager.deleteWallet(id)) saveAndRefresh();
    else UiHelper::showError(this, "Ví đang có giao dịch, không thể xóa.");
}
