#include "transaction_page.h"
#include "ui_transaction_page.h"
#include <QButtonGroup>
#include <QSignalBlocker>

void TransactionPage::clearQuickFilterChip() {
    if (!ui->quickFilterGroup->checkedButton()) return;
    ui->quickFilterGroup->setExclusive(false);
    ui->quickFilterGroup->checkedButton()->setChecked(false);
    ui->quickFilterGroup->setExclusive(true);
}

void TransactionPage::applyQuickDateRange(const QDate& from, const QDate& to) {
    QSignalBlocker checkBlocker(ui->chkDateFilter);
    QSignalBlocker fromBlocker(ui->dteFrom);
    QSignalBlocker toBlocker(ui->dteTo);
    bool active = from.isValid() && to.isValid();
    ui->chkDateFilter->setChecked(active);
    if (active) { ui->dteFrom->setDate(from); ui->dteTo->setDate(to); }
    else resetDates();
    ui->dteFrom->setEnabled(active);
    ui->dteTo->setEnabled(active);
    refreshData();
}

void TransactionPage::filterByDate(const QDate& date) {
    clearQuickFilterChip();
    applyQuickDateRange(date, date);
}

void TransactionPage::on_btnQuickAll_clicked() { applyQuickDateRange(QDate(), QDate()); }

void TransactionPage::on_btnQuickThisMonth_clicked() {
    QDate today = QDate::currentDate();
    QDate first(today.year(), today.month(), 1);
    applyQuickDateRange(first, first.addMonths(1).addDays(-1));
}

void TransactionPage::on_btnQuickLastMonth_clicked() {
    QDate thisMonth = QDate::currentDate();
    QDate first = QDate(thisMonth.year(), thisMonth.month(), 1).addMonths(-1);
    applyQuickDateRange(first, first.addMonths(1).addDays(-1));
}

void TransactionPage::on_btnQuickSevenDays_clicked() {
    QDate today = QDate::currentDate();
    applyQuickDateRange(today.addDays(-6), today);
}
