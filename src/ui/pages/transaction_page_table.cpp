#include "transaction_page.h"
#include "ui_transaction_page.h"
#include "../gui_helpers.h"
#include "../gui_icons_kpi.h"
#include "../gui_icons_nav.h"
#include "../gui_icons_wallet.h"
#include "../../core/app/money_manager.h"
#include <QSignalBlocker>

static QTableWidgetItem* noteItem(const Transaction* tx) {
    QString note = QString::fromStdString(tx->getNote());
    QTableWidgetItem* item = new QTableWidgetItem(note);
    item->setToolTip(note);
    item->setForeground(QColor("#5B6B8C"));
    return item;
}

void TransactionPage::loadWalletFilter() {
    QSignalBlocker blocker(ui->cboWallet);
    int id = ui->cboWallet->currentData().toInt();
    ui->cboWallet->clear();
    ui->cboWallet->addItem(GuiIcons::createWalletIcon(), "Ví: Tất cả", 0);
    const LinkedList<Wallet>& wallets = moneyManager.getWalletManager().getWallets();
    for (const Node<Wallet>* cur = wallets.getHead(); cur; cur = cur->next) {
        QString wName = QString::fromStdString(cur->data.getName());
        QString display = UiHelper::getWalletIcon(wName, (int)cur->data.getType()) + "  " + wName;
        ui->cboWallet->addItem(display, cur->data.getId());
    }
    int index = ui->cboWallet->findData(id);
    ui->cboWallet->setCurrentIndex(index < 0 ? 0 : index);
}

void TransactionPage::loadCategoryFilter() {
    QSignalBlocker blocker(ui->cboCategory);
    int id = ui->cboCategory->currentData().toInt();
    ui->cboCategory->clear();
    ui->cboCategory->addItem(GuiIcons::createTagIcon(), "Danh mục: Tất cả", 0);
    const LinkedList<Category>& categories = moneyManager.getCategoryManager().getCategories();
    for (const Node<Category>* cur = categories.getHead(); cur; cur = cur->next) {
        QString cName = QString::fromStdString(cur->data.getName());
        QString display = UiHelper::getCategoryIcon(cName) + "  " + cName;
        ui->cboCategory->addItem(display, cur->data.getId());
    }
    int index = ui->cboCategory->findData(id);
    ui->cboCategory->setCurrentIndex(index < 0 ? 0 : index);
}

void TransactionPage::loadFilterCombos() {
    loadWalletFilter();
    loadCategoryFilter();
}

void TransactionPage::addTxRow(const Transaction* tx) {
    int row = ui->tblTransactions->rowCount();
    ui->tblTransactions->insertRow(row);
    bool income = tx->getType() == TransactionType::INCOME;
    const Wallet* wallet = moneyManager.getWalletManager().findById(tx->getWalletId());
    const Category* category = moneyManager.getCategoryManager().findById(tx->getCategoryId());

    // Cột 0: Loại (TypeBadgeDelegate vẽ pill)
    QTableWidgetItem* type = new QTableWidgetItem(income ? "↑ Thu nhập" : "↓ Chi tiêu");
    type->setData(Qt::UserRole, tx->getId());

    // Cột 1: Số tiền (Bold, màu chuẩn prompt)
    QString prefix = income ? "↑ + " : "↓ - ";
    QTableWidgetItem* amount = new QTableWidgetItem(prefix + UiHelper::formatMoneyVND(tx->getAmount()));
    amount->setForeground(income ? UiHelper::colorIncome() : UiHelper::colorExpense());
    QFont amountFont = amount->font();
    amountFont.setBold(true);
    amount->setFont(amountFont);
    amount->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Cột 2: Ngày
    QTableWidgetItem* dateItem = new QTableWidgetItem(UiHelper::formatDate(tx->getDate()));
    dateItem->setForeground(QColor("#0B2A6B"));

    // Cột 3: Ví (Badge vuông nhỏ bo góc pastel + tên)
    QString wName = wallet ? QString::fromStdString(wallet->getName()) : "Ví khác";
    QTableWidgetItem* walletItem = new QTableWidgetItem(wName);
    walletItem->setIcon(QIcon(GuiIcons::createWalletBadgeIcon(wName, wallet ? (int)wallet->getType() : 0, 24)));
    walletItem->setForeground(QColor("#0B2A6B"));

    // Cột 4: Danh mục (Badge tròn màu pastel + tên)
    QString cName = category ? QString::fromStdString(category->getName()) : "Khác";
    QTableWidgetItem* catItem = new QTableWidgetItem(cName);
    catItem->setIcon(GuiIcons::createCategoryCircleIcon(cName, 24));
    catItem->setForeground(QColor("#0B2A6B"));

    ui->tblTransactions->setItem(row, 0, type);
    ui->tblTransactions->setItem(row, 1, amount);
    ui->tblTransactions->setItem(row, 2, dateItem);
    ui->tblTransactions->setItem(row, 3, walletItem);
    ui->tblTransactions->setItem(row, 4, catItem);
    ui->tblTransactions->setItem(row, 5, noteItem(tx));
}

bool TransactionPage::loadFilteredTransactions(LinkedList<Transaction*>& list) {
    QDate from = ui->dteFrom->date(), to = ui->dteTo->date();
    bool useDate = from.isValid() && to.isValid();
    if (useDate && from > to) {
        return false;
    }
    Date first = useDate ? Date(from.day(), from.month(), from.year()) : Date(0, 0, 0);
    Date last = useDate ? Date(to.day(), to.month(), to.year()) : Date(0, 0, 0);
    return moneyManager.getFilteredTransactions(list, first, last, ui->cboWallet->currentData().toInt(),
                                                ui->cboCategory->currentData().toInt(), ui->cboType->currentIndex());
}

void TransactionPage::updateSummary(const LinkedList<Transaction*>& list) {
    TransactionTotals totals = StatUtils::sumByType(list);
    ui->lblFilteredIncome->setText(UiHelper::formatMoneyVND(totals.income));
    ui->lblFilteredExpense->setText(UiHelper::formatMoneyVND(totals.expense));
    ui->lblFilteredCount->setText(QString::number(totals.count));
}

void TransactionPage::showEmptyTable() {
    ui->tblTransactions->setRowCount(1);
    ui->tblTransactions->setSpan(0, 0, 1, 6);
    bool dateInvalid = ui->dteFrom->isValidDate() && ui->dteTo->isValidDate() && (ui->dteFrom->date() > ui->dteTo->date());
    QString msg = dateInvalid
        ? "Khoảng ngày không hợp lệ: 'Từ ngày' phải trước hoặc bằng 'Đến ngày'"
        : "Không có giao dịch phù hợp với bộ lọc";
    QTableWidgetItem* item = new QTableWidgetItem(msg);
    item->setTextAlignment(Qt::AlignCenter);
    if (dateInvalid) item->setForeground(QColor("#E0173D"));
    else item->setForeground(QColor("#5B6B8C"));
    ui->tblTransactions->setItem(0, 0, item);
}

void TransactionPage::populateTable() {
    ui->tblTransactions->setRowCount(0);
    ui->tblTransactions->clearSpans();
    LinkedList<Transaction*> list, visible;
    if (!loadFilteredTransactions(list)) {
        showEmptyTable();
        updateSummary(visible);
        return;
    }
    for (const Node<Transaction*>* cur = list.getHead(); cur; cur = cur->next) {
        if (!cur->data) continue;
        if (!QString::fromStdString(cur->data->getNote()).contains(ui->edtKeyword->text().trimmed(), Qt::CaseInsensitive)) continue;
        visible.pushBack(cur->data);
        addTxRow(cur->data);
    }
    if (visible.isEmpty()) showEmptyTable();
    updateSummary(visible);
}
