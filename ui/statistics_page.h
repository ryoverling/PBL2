#pragma once

#include <QWidget>

class QComboBox;
class QDateEdit;
class QLabel;
class DonutChartWidget;
class BarChartWidget;

class TransactionManager;
class CategoryManager;

class StatisticsPage : public QWidget {
private:
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

    void refresh();
};
