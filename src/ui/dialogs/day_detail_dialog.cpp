#include "day_detail_dialog.h"
#include "ui_day_detail_dialog.h"
#include "../gui_helpers.h"
#include "../main_window.h"
#include "../../core/app/money_manager.h"
#include <QHeaderView>
#include <QLabel>
#include <QHBoxLayout>

DayDetailDialog::DayDetailDialog(MoneyManager& mm, const Date& date, MainWindow* mainWin, QWidget* parent)
    : QDialog(parent), ui(new Ui::DayDetailDialog), moneyManager(mm), targetDate(date), mainWindow(mainWin) {
    ui->setupUi(this);
    UiHelper::setupWindowTheme(this);

    auto* header = ui->tblDayTransactions->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setFixedHeight(42);
    ui->tblDayTransactions->verticalHeader()->setVisible(false);
    ui->tblDayTransactions->verticalHeader()->setDefaultSectionSize(48);

    int w = (Date::weekdayOfFirstDay(targetDate.month, targetDate.year) + targetDate.day - 1) % 7;
    if (w < 0) w += 7;
    static const char* weekdays[7] = {"Thứ Hai", "Thứ Ba", "Thứ Tư", "Thứ Năm", "Thứ Sáu", "Thứ Bảy", "Chủ Nhật"};
    QString dayName = (w >= 0 && w < 7) ? QString::fromUtf8(weekdays[w]) : "";
    ui->lblDateSubtitle->setText(QString("%1, %2").arg(dayName, QString::fromStdString(targetDate.toString())));

    populateData();
}

DayDetailDialog::~DayDetailDialog() {
    delete ui;
}

void DayDetailDialog::populateData() {
    LinkedList<Transaction*> list;
    moneyManager.getFilteredTransactions(list, targetDate, targetDate);

    long long income = 0;
    long long expense = 0;

    ui->tblDayTransactions->setRowCount(0);

    for (const Node<Transaction*>* cur = list.getHead(); cur; cur = cur->next) {
        const Transaction* tx = cur->data;
        if (!tx) continue;

        bool isInc = tx->getType() == TransactionType::INCOME;
        if (isInc) income += tx->getAmount();
        else expense += tx->getAmount();

        int row = ui->tblDayTransactions->rowCount();
        ui->tblDayTransactions->insertRow(row);
        ui->tblDayTransactions->setRowHeight(row, 48);

        // Cột 0: Badge Loại bo góc sạch sẽ, KHÔNG vẽ text đè hoặc gạch ngang
        QWidget* badgeContainer = new QWidget(this);
        badgeContainer->setStyleSheet("background: transparent;");
        QHBoxLayout* badgeLayout = new QHBoxLayout(badgeContainer);
        badgeLayout->setContentsMargins(6, 4, 6, 4);
        badgeLayout->setAlignment(Qt::AlignCenter);

        QLabel* lblBadge = new QLabel(isInc ? "Thu nhập" : "Chi tiêu", badgeContainer);
        lblBadge->setAlignment(Qt::AlignCenter);
        lblBadge->setFixedHeight(26);
        lblBadge->setFixedWidth(86);
        lblBadge->setStyleSheet(QString(
            "background-color: %1; color: %2; font-weight: 700; font-size: 12px; "
            "border-radius: 13px; border: none;"
        ).arg(isInc ? "#E7FAF3" : "#FFF0F3", isInc ? "#059669" : "#E11D48"));
        badgeLayout->addWidget(lblBadge);

        // Dummy item rỗng ở cột 0 để tránh delegate vẽ đè
        auto* emptyCol0 = new QTableWidgetItem();
        ui->tblDayTransactions->setItem(row, 0, emptyCol0);
        ui->tblDayTransactions->setCellWidget(row, 0, badgeContainer);

        // Cột 1: Danh mục & Ghi chú rõ ràng kèm tooltip
        const Category* cat = moneyManager.getCategoryManager().findById(tx->getCategoryId());
        QString catName = cat ? QString::fromStdString(cat->getName()) : "Khác";
        QString note = QString::fromStdString(tx->getNote());
        QString displayText = catName;
        if (!note.isEmpty()) {
            displayText += "  —  " + note;
        }
        auto* descItem = new QTableWidgetItem(displayText);
        descItem->setToolTip(QString("Danh mục: %1\nGhi chú: %2").arg(catName, note.isEmpty() ? "(Không có)" : note));
        descItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->tblDayTransactions->setItem(row, 1, descItem);

        // Cột 2: Ví
        const Wallet* w = moneyManager.getWalletManager().findById(tx->getWalletId());
        auto* walletItem = new QTableWidgetItem(w ? QString::fromStdString(w->getName()) : "Ví khác");
        walletItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->tblDayTransactions->setItem(row, 2, walletItem);

        // Cột 3: Số tiền
        QString sign = isInc ? "+ " : "- ";
        auto* amountItem = new QTableWidgetItem(sign + UiHelper::formatMoneyVND(tx->getAmount()));
        amountItem->setForeground(QColor(isInc ? "#059669" : "#E11D48"));
        amountItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QFont f = amountItem->font();
        f.setBold(true);
        amountItem->setFont(f);
        ui->tblDayTransactions->setItem(row, 3, amountItem);
    }

    if (list.isEmpty()) {
        ui->tblDayTransactions->setRowCount(1);
        ui->tblDayTransactions->setSpan(0, 0, 1, 4);
        auto* emptyItem = new QTableWidgetItem("Không có giao dịch nào trong ngày này");
        emptyItem->setTextAlignment(Qt::AlignCenter);
        emptyItem->setForeground(QColor("#536986"));
        QFont ef = emptyItem->font();
        ef.setItalic(true);
        emptyItem->setFont(ef);
        ui->tblDayTransactions->setItem(0, 0, emptyItem);
        ui->tblDayTransactions->setRowHeight(0, 60);
    }

    long long net = income - expense;
    ui->lblDayIncome->setText(UiHelper::formatMoneyVND(income));
    ui->lblDayExpense->setText(UiHelper::formatMoneyVND(expense));
    ui->lblDayNet->setText(UiHelper::formatMoneyVND(net));
    ui->lblDayNet->setStyleSheet(QString("color: %1;").arg(net > 0 ? "#12B886" : net < 0 ? "#EF4765" : "#1769EE"));
}

void DayDetailDialog::on_btnClose_clicked() {
    accept();
}

void DayDetailDialog::on_btnViewInTransactions_clicked() {
    accept();
    if (mainWindow) {
        mainWindow->showTransactionsWithDate(targetDate);
    }
}
