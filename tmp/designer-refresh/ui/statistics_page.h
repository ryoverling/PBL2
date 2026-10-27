#pragma once

#include <QWidget>

class QComboBox;
class QDateEdit;
class QLabel;
class DonutChartWidget;
class BarChartWidget;

class TransactionManager;
class CategoryManager;
namespace Ui { class StatisticsPage; }

class StatisticsPage : public QWidget {
private:
    Ui::StatisticsPage* ui;
    TransactionManager* transactions;
    CategoryManager* categories;

    QComboBox* periodCombo;
    QDateEdit* referenceDate;
    QDateEdit* customStart;
    QDateEdit* customEnd;
    QLabel* incomeLabel;
    QLabel* expenseLabel;
    QLabel* differenceLabel;
    DonutChartWidget* donutChart;
    BarChartWidget* barChart;

    QString startDate() const;
    QString endDate() const;
    void updatePeriodUi();

public:
    StatisticsPage(TransactionManager* transactions,
                   CategoryManager* categories,
                   QWidget* parent = nullptr);
    ~StatisticsPage();

    void refresh();
};
