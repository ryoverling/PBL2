#include "transaction_page.h"
#include "ui_transaction_page.h"
#include <QSignalBlocker>

void TransactionPage::resetDates() {
    QSignalBlocker first(ui->dteFrom);
    QSignalBlocker last(ui->dteTo);
    QDate today = QDate::currentDate();
    ui->dteFrom->setDate(QDate(today.year(), today.month(), 1));
    ui->dteTo->setDate(today);
}

void TransactionPage::on_btnResetFilter_clicked() {
    QSignalBlocker keyword(ui->edtKeyword);
    QSignalBlocker type(ui->cboType);
    QSignalBlocker wallet(ui->cboWallet);
    QSignalBlocker category(ui->cboCategory);
    QSignalBlocker dateFilter(ui->chkDateFilter);
    ui->edtKeyword->clear();
    ui->cboType->setCurrentIndex(0);
    ui->cboWallet->setCurrentIndex(0);
    ui->cboCategory->setCurrentIndex(0);
    ui->chkDateFilter->setChecked(false);
    resetDates();
    on_chkDateFilter_toggled(false);
    ui->btnQuickAll->setChecked(true);
}

void TransactionPage::on_chkDateFilter_toggled(bool checked) {
    clearQuickFilterChip();
    if (checked && (!ui->dteFrom->date().isValid() || !ui->dteTo->date().isValid())) {
        resetDates();
    }
    ui->dteFrom->setEnabled(checked);
    ui->dteTo->setEnabled(checked);
    QString tip = checked ? "" : QString::fromUtf8("Bật 'Lọc theo ngày' để chọn");
    ui->dteFrom->setToolTip(tip);
    ui->dteTo->setToolTip(tip);
    refreshData();
}

void TransactionPage::on_dteFrom_dateChanged(const QDate& from) {
    if (from.isValid() && ui->dteTo->date().isValid() && from > ui->dteTo->date()) {
        ui->dteTo->setDate(from);
    }
    clearQuickFilterChip();
    refreshData();
}

void TransactionPage::on_dteTo_dateChanged(const QDate& to) {
    if (to.isValid() && ui->dteFrom->date().isValid() && to < ui->dteFrom->date()) {
        ui->dteFrom->setDate(to);
    }
    clearQuickFilterChip();
    refreshData();
}
void TransactionPage::on_cboType_currentIndexChanged(int) { refreshData(); }
void TransactionPage::on_cboWallet_currentIndexChanged(int) { refreshData(); }
void TransactionPage::on_cboCategory_currentIndexChanged(int) { refreshData(); }
void TransactionPage::on_edtKeyword_textChanged(const QString&) { refreshData(); }
