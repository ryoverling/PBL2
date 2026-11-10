#include "transaction_dialog.h"
#include "ui_transaction_dialog.h"
#include "../gui_helpers.h"
#include "../../core/app/money_manager.h"
#include "../../core/model/transaction.h"
#include <QMessageBox>

void TransactionDialog::on_btnSave_clicked() {
    QString name = UiHelper::cleanName(ui->cboCategory->currentText());
    if (getAmount() <= 0 || ui->cboWallet->currentIndex() < 0) {
        UiHelper::showError(this, "Vui lòng nhập số tiền hợp lệ và chọn ví thanh toán.");
        return;
    }
    if (!ui->dteDate->isValidDate() || !getDate().isValid()) {
        UiHelper::showError(this, "Ngày giao dịch không hợp lệ. Vui lòng kiểm tra lại.");
        return;
    }
    if (!UiHelper::isValidName(name) || getNote().find('|') != std::string::npos) {
        UiHelper::showError(this, "Danh mục hoặc ghi chú chứa ký tự không hợp lệ.");
        return;
    }

    const Category* category = findCategory(name);
    resolvedCategoryName = category ? category->getName() : name.toStdString();

    // KIỂM TRA BẤT BIẾN NGHIỆP VỤ: SỐ DƯ KHÔNG ĐƯỢC ÂM TRÊN TOÀN BỘ LỊCH SỬ
    CategoryType catType = (getType() == TransactionType::INCOME) ? CategoryType::INCOME : CategoryType::EXPENSE;
    BalanceValidationResult valRes;

    if (editingTxId > 0) {
        valRes = moneyManager.validateUpdateTransaction(editingTxId, getAmount(), getDate(), getWalletId(),
                                                        resolvedCategoryName, catType);
    } else {
        Transaction* testTx = createTransaction(1, 1);
        valRes = moneyManager.validateAddTransaction(testTx);
        delete testTx;
    }

    if (!valRes.isValid) {
        QString title = "Không thể thực hiện giao dịch";
        if (valRes.errorCode == ValidationErrorCode::INSUFFICIENT_FUNDS) {
            title = "Số dư không đủ";
        } else if (valRes.errorCode == ValidationErrorCode::WALLET_ALREADY_NEGATIVE) {
            title = "Ví đang có số dư âm";
        } else if (valRes.errorCode == ValidationErrorCode::WOULD_GO_NEGATIVE_ON_DATE) {
            title = "Số dư sẽ bị âm trong lịch sử";
        }
        QMessageBox box(QMessageBox::Warning, title,
                        QString::fromStdString(valRes.errorMessage), QMessageBox::Ok, this);
        box.setStyleSheet("QLabel { font-size: 13px; color: #102748; line-height: 140%; }");
        box.exec();
        return; // KHÔNG chấp nhận, KHÔNG đóng form, giữ nguyên nội dung nhập
    }

    accept();
}

long long TransactionDialog::getAmount() const {
    QString text = ui->edtAmount->text().trimmed();
    text.remove('.');
    text.remove(',');
    text.remove(' ');
    bool valid = false;
    long long amount = text.toLongLong(&valid);
    return valid ? amount : 0;
}

Date TransactionDialog::getDate() const {
    QDate date = ui->dteDate->date();
    return Date(date.day(), date.month(), date.year());
}

TransactionType TransactionDialog::getType() const {
    return (ui->btnTypeIncome && ui->btnTypeIncome->isChecked()) ? TransactionType::INCOME : TransactionType::EXPENSE;
}

int TransactionDialog::getWalletId() const {
    return ui->cboWallet->currentData().toInt();
}

std::string TransactionDialog::getNote() const {
    return ui->edtNote->text().trimmed().toStdString();
}

void TransactionDialog::on_btnCancel_clicked() {
    reject();
}

Transaction* TransactionDialog::createTransaction(int id, int categoryId) const {
    if (getType() == TransactionType::INCOME) {
        return new Income(id, getAmount(), getDate(), getNote(), getWalletId(), categoryId);
    }
    return new Expense(id, getAmount(), getDate(), getNote(), getWalletId(), categoryId);
}
