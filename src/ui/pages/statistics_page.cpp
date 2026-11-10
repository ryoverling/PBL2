#include "statistics_page.h"
#include "ui_statistics_page.h"
#include "../widgets/date_picker_popup.h"
#include "../gui_helpers.h"
#include "../../core/app/money_manager.h"
#include <QSignalBlocker>
#include <QStyle>

StatisticsPage::StatisticsPage(MoneyManager& mm, QWidget *parent)
    : QWidget(parent), ui(new Ui::StatisticsPage), moneyManager(mm) {
    ui->setupUi(this);

    ui->tblCategories->horizontalHeader()->hide();
    ui->tblCategories->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tblCategories->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tblCategories->verticalHeader()->setVisible(false);
    ui->tblCategories->setSelectionMode(QAbstractItemView::NoSelection);

    Date today = Date::today();
    ui->monthPicker->setMonthYear(today.month, today.year);
    connect(ui->monthPicker, &MonthPickerWidget::monthYearChanged, this, &StatisticsPage::on_monthPicker_monthYearChanged);

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
    ui->cardNet->setProperty("negative", net < 0);
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
