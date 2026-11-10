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
    QPixmap marker(14, 14);
    marker.fill(Qt::transparent);
    QPainter painter(&marker);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(ui->donutWidget->sliceColor(index));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(marker.rect().adjusted(1, 1, -1, -1), 3, 3);

    QString rawName = ui->donutWidget->sliceName(index);
    QString displayName = UiHelper::getCategoryIcon(rawName) + "  " + rawName;

    auto* name = new QTableWidgetItem(QIcon(marker), displayName);
    name->setToolTip(rawName);
    ui->tblCategories->setItem(row, 0, name);

    auto* amount = new QTableWidgetItem(UiHelper::formatMoneyVND(ui->donutWidget->sliceAmount(index)));
    amount->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QFont font = amount->font();
    font.setBold(true);
    amount->setFont(font);
    ui->tblCategories->setItem(row, 1, amount);
}

void StatisticsPage::updateCategories() {
    LinkedList<CategoryExpense> categories;
    moneyManager.getExpenseByCategory(selectedMonth(), selectedYear(), categories);
    long long income = 0, expense = 0;
    moneyManager.getMonthlyTotals(selectedMonth(), selectedYear(), income, expense);
    ui->donutWidget->setData(categories, expense);
    ui->tblCategories->setRowCount(0);
    ui->tblCategories->clearSpans();
    for (int index = 0; index < ui->donutWidget->sliceCount(); ++index) addCategoryRow(index);
    if (categories.isEmpty()) {
        ui->tblCategories->setRowCount(1);
        ui->tblCategories->setSpan(0, 0, 1, 2);
        ui->tblCategories->setItem(0, 0, new QTableWidgetItem("Chưa có khoản chi"));
    }
    int listHeight = qMin(6, ui->tblCategories->rowCount())
                   * ui->tblCategories->verticalHeader()->defaultSectionSize() + 4;
    ui->tblCategories->setFixedHeight(listHeight);
    ui->frmDonut->setMinimumHeight(qMax(310, listHeight + 144));
    QString top = categories.isEmpty() ? "Chưa có khoản chi" : QString::fromStdString(categories.getHead()->data.categoryName);
    ui->lblTopCategory->setText("Danh mục chi nhiều nhất: " + (categories.isEmpty() ? top : UiHelper::getCategoryIcon(top) + " " + top));
}
