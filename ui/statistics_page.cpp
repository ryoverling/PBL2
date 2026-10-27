#include "statistics_page.h"
#include "ui/ui_statistics_page.h"

#include "chart_widgets.h"
#include "ui_helpers.h"

#include "../manager/transaction_manager.h"
#include "../manager/category_manager.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>


StatisticsPage::StatisticsPage(TransactionManager* transactions,
                               CategoryManager* categories,
                               QWidget* parent)
    : QWidget(parent),
      ui(new Ui::StatisticsPage),
      transactions(transactions),
      categories(categories) {
    ui->setupUi(this);
    periodCombo = ui->periodCombo;
    referenceDate = ui->referenceDate;
    customStart = ui->customStart;
    customEnd = ui->customEnd;
    incomeLabel = ui->incomeLabel;
    expenseLabel = ui->expenseLabel;
    differenceLabel = ui->differenceLabel;
    donutChart = ui->donutChart;
    barChart = ui->barChart;
    periodCombo->addItem("Tuần này", 0);
    periodCombo->addItem("Tháng này", 1);
    periodCombo->addItem("Năm nay", 2);
    periodCombo->addItem("Tùy chọn", 3);
    periodCombo->setCurrentIndex(1);

    referenceDate->setDate(QDate::currentDate());
    customStart->setDate(QDate::currentDate().addMonths(-1));
    customEnd->setDate(QDate::currentDate());

    auto refreshFn = [this]() { refresh(); };
    connect(periodCombo, &QComboBox::currentIndexChanged, this, [this]() {
        updatePeriodUi();
        refresh();
    });
    connect(referenceDate, &QDateEdit::dateChanged, this, refreshFn);
    connect(customStart, &QDateEdit::dateChanged, this, refreshFn);
    connect(customEnd, &QDateEdit::dateChanged, this, refreshFn);

    updatePeriodUi();
    refresh();
}

StatisticsPage::~StatisticsPage() {
    delete ui;
}

QString StatisticsPage::startDate() const {
    QDate ref = referenceDate->date();
    int mode = periodCombo->currentData().toInt();

    if (mode == 0) return ref.addDays(1 - ref.dayOfWeek()).toString("yyyy-MM-dd");
    if (mode == 1) return QDate(ref.year(), ref.month(), 1).toString("yyyy-MM-dd");
    if (mode == 2) return QDate(ref.year(), 1, 1).toString("yyyy-MM-dd");
    return customStart->date().toString("yyyy-MM-dd");
}

QString StatisticsPage::endDate() const {
    QDate ref = referenceDate->date();
    int mode = periodCombo->currentData().toInt();

    if (mode == 0) return ref.addDays(7 - ref.dayOfWeek()).toString("yyyy-MM-dd");
    if (mode == 1) {
        return QDate(ref.year(), ref.month(), 1)
            .addMonths(1).addDays(-1)
            .toString("yyyy-MM-dd");
    }
    if (mode == 2) return QDate(ref.year(), 12, 31).toString("yyyy-MM-dd");
    return customEnd->date().toString("yyyy-MM-dd");
}

void StatisticsPage::updatePeriodUi() {
    bool custom = periodCombo->currentData().toInt() == 3;
    referenceDate->setVisible(!custom);
    customStart->setVisible(custom);
    customEnd->setVisible(custom);
}

void StatisticsPage::refresh() {
    string start = startDate().toStdString();
    string end = endDate().toStdString();

    double income = transactions->getTotalIncome(start, end);
    double expense = transactions->getTotalExpense(start, end);

    incomeLabel->setText(UiHelpers::money(income));
    expenseLabel->setText(UiHelpers::money(expense));
    differenceLabel->setText(UiHelpers::money(income - expense));

    QVector<QColor> palette = {
        QColor("#6c4ed9"), QColor("#f09a58"), QColor("#51a8a2"),
        QColor("#d56486"), QColor("#5c85d6"), QColor("#d1a83d"),
        QColor("#8d6ab8"), QColor("#7c9a50")
    };

    QVector<ChartSlice> slices;
    int colorIndex = 0;
    categories->getCategories().forEach([&](const Category& category) {
        double value = transactions->getCategoryExpense(category.getId(), start, end);
        if (value <= 0) return;
        slices.push_back({
            QString::fromStdString(category.getName()),
            value,
            palette[colorIndex % palette.size()]
        });
        colorIndex++;
    });
    donutChart->setData(slices);

    QStringList monthLabels;
    QVector<double> monthValues;
    QDate now = QDate::currentDate();

    for (int offset = 5; offset >= 0; --offset) {
        QDate month = QDate(now.year(), now.month(), 1).addMonths(-offset);
        QDate last = month.addMonths(1).addDays(-1);

        string monthStart = month.toString("yyyy-MM-dd").toStdString();
        string monthEnd = last.toString("yyyy-MM-dd").toStdString();

        monthLabels << month.toString("MM/yy");
        monthValues << transactions->getTotalExpense(monthStart, monthEnd);
    }

    barChart->setData(monthLabels, monthValues);
}
