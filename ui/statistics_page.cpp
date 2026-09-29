#include "statistics_page.h"

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

#include <map>

namespace {
    QFrame* makeSummaryCard(const QString& caption, QLabel*& valueLabel) {
        QFrame* card = new QFrame();
        card->setObjectName("card");
        QVBoxLayout* layout = new QVBoxLayout(card);
        layout->setContentsMargins(18, 16, 18, 16);

        QLabel* title = new QLabel(caption);
        title->setObjectName("mutedLabel");

        valueLabel = new QLabel("0 đ");
        valueLabel->setStyleSheet("font-size: 23px; font-weight: 800;");

        layout->addWidget(title);
        layout->addWidget(valueLabel);
        return card;
    }
}

StatisticsPage::StatisticsPage(TransactionManager* transactions,
                               CategoryManager* categories,
                               QWidget* parent)
    : QWidget(parent),
      transactions(transactions),
      categories(categories) {

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 28);
    root->setSpacing(16);

    QHBoxLayout* header = new QHBoxLayout();
    QLabel* title = new QLabel("Thống kê");
    title->setObjectName("pageTitle");

    periodCombo = new QComboBox();
    periodCombo->addItem("Tuần này", 0);
    periodCombo->addItem("Tháng này", 1);
    periodCombo->addItem("Năm nay", 2);
    periodCombo->addItem("Tùy chọn", 3);
    periodCombo->setCurrentIndex(1);

    referenceDate = new QDateEdit(QDate::currentDate());
    referenceDate->setCalendarPopup(true);
    referenceDate->setDisplayFormat("dd/MM/yyyy");

    customStart = new QDateEdit(QDate::currentDate().addMonths(-1));
    customStart->setCalendarPopup(true);
    customStart->setDisplayFormat("dd/MM/yyyy");

    customEnd = new QDateEdit(QDate::currentDate());
    customEnd->setCalendarPopup(true);
    customEnd->setDisplayFormat("dd/MM/yyyy");

    header->addWidget(title);
    header->addStretch();
    header->addWidget(periodCombo);
    header->addWidget(referenceDate);
    header->addWidget(customStart);
    header->addWidget(customEnd);

    QGridLayout* summaryGrid = new QGridLayout();
    summaryGrid->setHorizontalSpacing(14);
    summaryGrid->addWidget(makeSummaryCard("Thu nhập", incomeLabel), 0, 0);
    summaryGrid->addWidget(makeSummaryCard("Chi tiêu", expenseLabel), 0, 1);
    summaryGrid->addWidget(makeSummaryCard("Chênh lệch", differenceLabel), 0, 2);

    QGridLayout* charts = new QGridLayout();
    charts->setHorizontalSpacing(14);

    QFrame* donutCard = new QFrame();
    donutCard->setObjectName("card");
    QVBoxLayout* donutLayout = new QVBoxLayout(donutCard);
    donutLayout->setContentsMargins(18, 16, 18, 16);
    QLabel* donutTitle = new QLabel("Chi tiêu theo danh mục");
    donutTitle->setObjectName("sectionTitle");
    donutChart = new DonutChartWidget();
    donutLayout->addWidget(donutTitle);
    donutLayout->addWidget(donutChart);

    QFrame* barCard = new QFrame();
    barCard->setObjectName("card");
    QVBoxLayout* barLayout = new QVBoxLayout(barCard);
    barLayout->setContentsMargins(18, 16, 18, 16);
    QLabel* barTitle = new QLabel("Chi tiêu 6 tháng gần nhất");
    barTitle->setObjectName("sectionTitle");
    barChart = new BarChartWidget();
    barLayout->addWidget(barTitle);
    barLayout->addWidget(barChart);

    charts->addWidget(donutCard, 0, 0);
    charts->addWidget(barCard, 0, 1);
    charts->setColumnStretch(0, 1);
    charts->setColumnStretch(1, 1);

    root->addLayout(header);
    root->addLayout(summaryGrid);
    root->addLayout(charts);

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

    std::map<int, double> totals;
    transactions->getTransactions().forEach([&](Transaction* const& t) {
        if (t == nullptr || t->getType() != TransactionType::EXPENSE) return;
        QString date = QString::fromStdString(t->getDateTime()).left(10);
        if (date < QString::fromStdString(start) || date > QString::fromStdString(end)) return;
        totals[t->getCategoryId()] += t->getAmount();
    });

    QVector<QColor> palette = {
        QColor("#6c4ed9"), QColor("#f09a58"), QColor("#51a8a2"),
        QColor("#d56486"), QColor("#5c85d6"), QColor("#d1a83d"),
        QColor("#8d6ab8"), QColor("#7c9a50")
    };

    QVector<ChartSlice> slices;
    int colorIndex = 0;
    for (const auto& [categoryId, value] : totals) {
        const Category* category = categories->findById(categoryId);
        slices.push_back({
            category ? QString::fromStdString(category->getName()) : "Không rõ",
            value,
            palette[colorIndex % palette.size()]
        });
        colorIndex++;
    }
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
