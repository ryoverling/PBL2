#include "statistics_page.h"
#include "ui_statistics_page.h"
#include "../widgets/date_picker_popup.h"
#include "../gui_helpers.h"
#include "../gui_icons.h"
#include "../../core/app/money_manager.h"
#include <QSignalBlocker>
#include <QStyle>
#include <cmath>

StatisticsPage::StatisticsPage(MoneyManager& mm, QWidget *parent)
    : QWidget(parent), ui(new Ui::StatisticsPage), moneyManager(mm) {
    ui->setupUi(this);

    // Thiết lập biểu tượng và đổ bóng cho các thẻ
    ui->lblHeaderIcon->setPixmap(GuiIcons::createStatisticsHeaderIcon(44));
    ui->iconIncomeBadge->setPixmap(GuiIcons::createKpiBadge("income", 44));
    ui->iconExpenseBadge->setPixmap(GuiIcons::createKpiBadge("expense", 44));
    ui->iconNetBadge->setPixmap(GuiIcons::createScaleBadge(44));
    ui->lblIncomeSparkline->setPixmap(GuiIcons::createWaveSparkline(true, 80, 36));
    ui->lblExpenseSparkline->setPixmap(GuiIcons::createWaveSparkline(false, 80, 36));

    UiHelper::applyCardShadow(ui->cardIncome);
    UiHelper::applyCardShadow(ui->cardExpense);
    UiHelper::applyCardShadow(ui->cardNet);
    UiHelper::applyCardShadow(ui->frmChart);
    UiHelper::applyCardShadow(ui->frmDonut);

    ui->tblCategories->horizontalHeader()->hide();
    ui->tblCategories->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tblCategories->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tblCategories->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tblCategories->verticalHeader()->setVisible(false);
    ui->tblCategories->verticalHeader()->setDefaultSectionSize(52);
    ui->tblCategories->setSelectionMode(QAbstractItemView::NoSelection);

    Date today = Date::today();
    ui->monthPicker->setMonthYear(today.month, today.year);

    refreshData();
}

StatisticsPage::~StatisticsPage() { delete ui; }

int StatisticsPage::selectedMonth() const { return ui->monthPicker->month(); }
int StatisticsPage::selectedYear() const { return ui->monthPicker->year(); }

void StatisticsPage::setInitialPeriod(int month, int year) {
    if (initialPeriodSet) return;
    initialPeriodSet = true;
    ui->monthPicker->setMonthYear(month, year);
    refreshData();
}

void StatisticsPage::updateCards() {
    int month = selectedMonth();
    int year = selectedYear();
    long long income = 0, expense = 0;
    moneyManager.getMonthlyTotals(month, year, income, expense);
    ui->lblTotalIncome->setText(UiHelper::formatMoneyVND(income));
    ui->lblTotalExpense->setText(UiHelper::formatMoneyVND(expense));

    long long net = moneyManager.getMonthlyDifference(month, year);
    ui->lblNetTotal->setText(UiHelper::formatMoneyVND(net));

    QString state = (net > 0) ? "positive" : (net < 0 ? "negative" : "zero");
    ui->cardNet->setProperty("netState", state);
    ui->lblNetTotal->setProperty("netState", state);

    ui->cardNet->style()->unpolish(ui->cardNet);
    ui->cardNet->style()->polish(ui->cardNet);
    ui->lblNetTotal->style()->unpolish(ui->lblNetTotal);
    ui->lblNetTotal->style()->polish(ui->lblNetTotal);
}

void StatisticsPage::updateChart() {
    MonthStat stats[6];
    moneyManager.get6MonthsStats(selectedMonth(), selectedYear(), stats);
    ui->chartWidget->setData(stats);
}

void StatisticsPage::refreshData() {
    updateCards();
    updateChart();
    updateCategories();
    updateComparison();
}

void StatisticsPage::on_monthPicker_monthYearChanged(int, int) {
    refreshData();
}

void StatisticsPage::updateComparison() {
    ExpenseComparison comparison = moneyManager.compareExpenseWithPreviousMonth(selectedMonth(), selectedYear());
    QString text = "Chưa có dữ liệu tháng trước";
    if (comparison.canCompare && comparison.diffAmount == 0) text = "Không đổi";
    else if (comparison.canCompare) {
        double pct = std::abs(comparison.diffPercent);
        QString pctStr = pct == std::floor(pct) ? QString::number(static_cast<long long>(pct))
                                                : QString::number(pct, 'f', 1);
        text = QString("%1 %2%").arg(QString::fromStdString(comparison.message), pctStr);
    }
    ui->lblComparison->setText("So với tháng trước: " + text);
}
