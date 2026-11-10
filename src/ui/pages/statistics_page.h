#pragma once

#include <QWidget>

class MoneyManager;

namespace Ui {
class StatisticsPage;
}

// Trang Thống Kê
class StatisticsPage : public QWidget {
    Q_OBJECT

public:
    explicit StatisticsPage(MoneyManager& mm, QWidget *parent = nullptr);
    ~StatisticsPage();

    void refreshData();
    void setInitialPeriod(int month, int year);
    int selectedMonth() const;
    int selectedYear() const;

private slots:
    void on_monthPicker_monthYearChanged(int month, int year);

private:
    Ui::StatisticsPage *ui;
    MoneyManager& moneyManager;
    bool initialPeriodSet = false;

    void updateCards();
    void updateChart();
    void updateCategories();
    void addCategoryRow(int index);
    void updateComparison();
};
