#include "statistics_page.h"
#include "ui_statistics_page.h"
#include "../gui_helpers.h"
#include "../widgets/donut_widget.h"
#include "../../core/app/money_manager.h"
#include <QPainter>
#include <QPixmap>

void StatisticsPage::addCategoryRow(int index) {
    int row = ui->tblCategories->rowCount();
    ui->tblCategories->insertRow(row);

    QString rawName = ui->donutWidget->sliceName(index);
    QIcon icon = UiHelper::createCategoryCircleIcon(rawName, 26);

    // Cột 0: Icon + Tên danh mục (kèm vương miện nếu chi nhiều nhất)
    QString displayName = rawName;
    if (index == 0) displayName += "  👑";
    auto* nameItem = new QTableWidgetItem(icon, displayName);
    nameItem->setToolTip(rawName);
    ui->tblCategories->setItem(row, 0, nameItem);

    // Cột 1: Số tiền chi
    long long amt = ui->donutWidget->sliceAmount(index);
    auto* amountItem = new QTableWidgetItem(UiHelper::formatMoneyVND(amt));
    amountItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QFont font = amountItem->font();
    font.setBold(true);
    amountItem->setFont(font);
    ui->tblCategories->setItem(row, 1, amountItem);

    // Cột 2: Tỷ trọng phần trăm
    int pct = ui->donutWidget->slicePercentage(index);
    auto* pctItem = new QTableWidgetItem(QString::number(pct) + "%");
    pctItem->setTextAlignment(Qt::AlignCenter);
    pctItem->setForeground(QColor("#5B6B8C"));
    ui->tblCategories->setItem(row, 2, pctItem);
}

void StatisticsPage::updateCategories() {
    LinkedList<CategoryExpense> categories;
    moneyManager.getExpenseByCategory(selectedMonth(), selectedYear(), categories);
    long long income = 0, expense = 0;
    moneyManager.getMonthlyTotals(selectedMonth(), selectedYear(), income, expense);
    ui->donutWidget->setData(categories, expense);

    ui->tblCategories->setRowCount(0);
    ui->tblCategories->clearSpans();
    for (int index = 0; index < ui->donutWidget->sliceCount(); ++index) {
        addCategoryRow(index);
    }

    if (categories.isEmpty()) {
        ui->tblCategories->setRowCount(1);
        ui->tblCategories->setSpan(0, 0, 1, 3);
        auto* empty = new QTableWidgetItem("Chưa có khoản chi trong tháng này");
        empty->setTextAlignment(Qt::AlignCenter);
        ui->tblCategories->setItem(0, 0, empty);
        ui->lblTopCategory->setText("Danh mục chi nhiều nhất: Chưa có dữ liệu");
    } else {
        QString top = QString::fromStdString(categories.getHead()->data.categoryName);
        ui->lblTopCategory->setText("Danh mục chi nhiều nhất: " + UiHelper::getCategoryIcon(top) + " " + top);
    }

    int listHeight = qMin(6, ui->tblCategories->rowCount()) * 52 + 4;
    ui->tblCategories->setFixedHeight(listHeight);
    ui->frmDonut->setMinimumHeight(qMax(260, listHeight + 110));
}
