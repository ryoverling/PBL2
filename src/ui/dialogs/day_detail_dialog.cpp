#include "day_detail_dialog.h"
#include "ui_day_detail_dialog.h"
#include "../gui_helpers.h"
#include "../gui_icons_kpi.h"
#include "../gui_icons_nav.h"
#include "../main_window.h"
#include "../../core/app/money_manager.h"
#include <QHeaderView>
#include <QLabel>
#include <QHBoxLayout>

void DayDetailDialog::setupKpiBadges() {
    UiHelper::applyCardShadows({ui->cardIncome, ui->cardExpense, ui->cardNet});

    QLabel* badgeInc = new QLabel(ui->cardIncome);
    badgeInc->setPixmap(GuiIcons::createKpiBadge("income", 28));
    ui->incomeLayout->insertWidget(0, badgeInc, 0, Qt::AlignRight);

    QLabel* badgeExp = new QLabel(ui->cardExpense);
    badgeExp->setPixmap(GuiIcons::createKpiBadge("expense", 28));
    ui->expenseLayout->insertWidget(0, badgeExp, 0, Qt::AlignRight);

    QLabel* badgeNet = new QLabel(ui->cardNet);
    badgeNet->setPixmap(GuiIcons::createScaleBadge(28));
    ui->netLayout->insertWidget(0, badgeNet, 0, Qt::AlignRight);
}

QWidget* DayDetailDialog::createTypeBadgeWidget(bool isIncome) {
    QWidget* container = new QWidget(this);
    container->setObjectName("dayBadgeContainer");
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->setAlignment(Qt::AlignCenter);

    QLabel* lblBadge = new QLabel(isIncome ? "Thu nhập" : "Chi tiêu", container);
    lblBadge->setObjectName(isIncome ? "lblDayBadgeIncome" : "lblDayBadgeExpense");
    lblBadge->setAlignment(Qt::AlignCenter);
    lblBadge->setFixedHeight(26);
    lblBadge->setFixedWidth(86);
    layout->addWidget(lblBadge);
    return container;
}

DayDetailDialog::DayDetailDialog(MoneyManager& mm, const Date& date, MainWindow* mainWin, QWidget* parent)
    : QDialog(parent), ui(new Ui::DayDetailDialog), moneyManager(mm), targetDate(date), mainWindow(mainWin) {
    ui->setupUi(this);
    UiHelper::setupWindowTheme(this);

    QLabel* lblCalIcon = new QLabel(this);
    lblCalIcon->setFixedSize(44, 44);
    lblCalIcon->setPixmap(GuiIcons::createCalendarBadge(44));
    ui->headerLayout->insertWidget(0, lblCalIcon);

    ui->btnViewInTransactions->setIcon(QIcon(GuiIcons::createListIcon(16, QColor("#1E6FFF"))));
    ui->btnClose->setText("✕ Đóng");

    setupKpiBadges();

    ui->tblDayTransactions->setShowGrid(false);
    QHeaderView* header = ui->tblDayTransactions->horizontalHeader();
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

void DayDetailDialog::addDayRow(const Transaction* tx, int row) {
    bool isInc = tx->getType() == TransactionType::INCOME;
    ui->tblDayTransactions->insertRow(row);
    ui->tblDayTransactions->setRowHeight(row, 48);

    // Cột 0: Badge Loại bo góc mềm mại
    ui->tblDayTransactions->setItem(row, 0, new QTableWidgetItem());
    ui->tblDayTransactions->setCellWidget(row, 0, createTypeBadgeWidget(isInc));

    // Cột 1: Danh mục & Ghi chú
    const Category* cat = moneyManager.getCategoryManager().findById(tx->getCategoryId());
    QString catName = cat ? QString::fromStdString(cat->getName()) : "Khác";
    QString note = QString::fromStdString(tx->getNote());
    QString displayText = note.isEmpty() ? catName : (catName + "  —  " + note);
    QTableWidgetItem* descItem = new QTableWidgetItem(displayText);
    descItem->setToolTip(QString("Danh mục: %1\nGhi chú: %2").arg(catName, note.isEmpty() ? "(Không có)" : note));
    descItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->tblDayTransactions->setItem(row, 1, descItem);

    // Cột 2: Ví
    const Wallet* w = moneyManager.getWalletManager().findById(tx->getWalletId());
    QTableWidgetItem* walletItem = new QTableWidgetItem(w ? QString::fromStdString(w->getName()) : "Ví khác");
    walletItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->tblDayTransactions->setItem(row, 2, walletItem);

    // Cột 3: Số tiền
    QString sign = isInc ? "+ " : "- ";
    QTableWidgetItem* amountItem = new QTableWidgetItem(sign + UiHelper::formatMoneyVND(tx->getAmount()));
    amountItem->setForeground(isInc ? UiHelper::colorIncome() : UiHelper::colorExpense());
    amountItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QFont f = amountItem->font();
    f.setBold(true);
    amountItem->setFont(f);
    ui->tblDayTransactions->setItem(row, 3, amountItem);
}

void DayDetailDialog::updateSummaryTotals(long long income, long long expense) {
    long long net = income - expense;
    ui->lblDayIncome->setText(UiHelper::formatMoneyVND(income));
    ui->lblDayExpense->setText(UiHelper::formatMoneyVND(expense));
    ui->lblDayNet->setText(UiHelper::formatMoneyVND(net));
    QString netColor = "#1E6FFF";
    if (net > 0) {
        netColor = "#0E8F4F";
    } else if (net < 0) {
        netColor = "#E0173D";
    }
    ui->lblDayNet->setStyleSheet(QString("color: %1;").arg(netColor));
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

        if (tx->getType() == TransactionType::INCOME) income += tx->getAmount();
        else expense += tx->getAmount();

        addDayRow(tx, ui->tblDayTransactions->rowCount());
    }

    if (list.isEmpty()) {
        ui->tblDayTransactions->setRowCount(1);
        ui->tblDayTransactions->setSpan(0, 0, 1, 4);
        QTableWidgetItem* emptyItem = new QTableWidgetItem("Không có giao dịch nào trong ngày này");
        emptyItem->setTextAlignment(Qt::AlignCenter);
        emptyItem->setForeground(QColor("#5B6B8C"));
        QFont ef = emptyItem->font();
        ef.setItalic(true);
        emptyItem->setFont(ef);
        ui->tblDayTransactions->setItem(0, 0, emptyItem);
        ui->tblDayTransactions->setRowHeight(0, 60);
    }

    updateSummaryTotals(income, expense);
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
