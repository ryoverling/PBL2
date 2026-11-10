#include "transaction_page.h"
#include "../gui_helpers.h"
#include "../dialogs/transaction_dialog.h"
#include "../../core/app/money_manager.h"

void TransactionPage::on_btnAdd_clicked() {
    TransactionDialog dialog(moneyManager, -1, this);
    if (dialog.exec() != QDialog::Accepted) return;
    int id = moneyManager.getTransactionManager().getNextId();
    Transaction* transaction = dialog.createTransaction(id, 0);
    CategoryType type = dialog.getType() == TransactionType::INCOME ? CategoryType::INCOME : CategoryType::EXPENSE;
    if (moneyManager.addTransaction(transaction, dialog.getCategoryName(), type)) {
        saveAndRefresh();
    } else {
        delete transaction;
        UiHelper::showError(this, "Không thể thêm giao dịch (số dư ví không đủ hoặc dữ liệu không hợp lệ).");
    }
}

bool TransactionPage::updateTransaction(int id, const TransactionDialog& dialog) {
    const Transaction* old = moneyManager.getTransactionManager().findById(id);
    if (!old) return false;
    CategoryType type = dialog.getType() == TransactionType::INCOME ? CategoryType::INCOME : CategoryType::EXPENSE;
    if (old->getType() == dialog.getType()) {
        return moneyManager.updateTransaction(id, dialog.getAmount(), dialog.getDate(), dialog.getNote(),
                                               dialog.getWalletId(), dialog.getCategoryName(), type);
    }
    Transaction* replacement = dialog.createTransaction(id, 0);
    bool changed = moneyManager.deleteTransaction(id) && moneyManager.addTransaction(replacement, dialog.getCategoryName(), type);
    if (!changed) delete replacement;
    return changed;
}

void TransactionPage::on_btnEdit_clicked() {
    int id = getSelectedTxId();
    if (id < 0) return;
    TransactionDialog dialog(moneyManager, id, this);
    if (dialog.exec() != QDialog::Accepted) return;
    if (updateTransaction(id, dialog)) {
        saveAndRefresh();
    } else {
        UiHelper::showError(this, "Không thể cập nhật giao dịch (vi phạm bất biến số dư hoặc lỗi dữ liệu).");
    }
}

void TransactionPage::on_btnDelete_clicked() {
    int id = getSelectedTxId();
    if (id < 0) return;
    const Transaction* tx = moneyManager.getTransactionManager().findById(id);
    if (!tx) return;

    // KIỂM TRA BẤT BIẾN NGHIỆP VỤ: Xóa khoản thu không được làm âm số dư ví trong tương lai
    BalanceValidationResult valRes = moneyManager.validateDeleteTransaction(id);
    if (!valRes.isValid) {
        UiHelper::showError(this, QString::fromStdString(valRes.errorMessage));
        return;
    }

    QString type = tx->getType() == TransactionType::INCOME ? "Thu" : "Chi";
    QString note = tx->getNote().empty() ? "Không có" : QString::fromStdString(tx->getNote());
    QString message = QString("%1 | %2 | %3\nGhi chú: %4\n\nXóa giao dịch này?")
        .arg(type).arg(UiHelper::formatDate(tx->getDate())).arg(UiHelper::formatMoneyVND(tx->getAmount())).arg(note);
    if (!UiHelper::askConfirmation(this, "Xác nhận xóa", message)) return;
    if (moneyManager.deleteTransaction(id)) {
        saveAndRefresh();
    } else {
        UiHelper::showError(this, "Không thể xóa giao dịch.");
    }
}
