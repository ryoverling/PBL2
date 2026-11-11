#pragma once

#include <QWidget>
#include <string>
#include "../dialogs/plan_dialog.h"

class MoneyManager;
class MainWindow;

namespace Ui {
class DashboardPage;
}

// Trang Tổng Quan Thu Chi
class DashboardPage : public QWidget {
    Q_OBJECT

public:
    explicit DashboardPage(MoneyManager& mm, const std::string& dataDir,
                           MainWindow* mainWin = nullptr, QWidget *parent = nullptr);
    ~DashboardPage();

    void refreshData();
    int selectedMonth() const;
    int selectedYear() const;

private slots:
    void on_btnViewAll_clicked();
    void on_btnAddTransaction_clicked();
    void on_monthPicker_monthYearChanged(int month, int year);
    void on_btnEditPlan_clicked();
    void on_btnEye_clicked();
    void on_calDaily_dayClicked(int day);

private:
    Ui::DashboardPage *ui;
    MoneyManager& moneyManager;
    std::string dataDirectory;
    MainWindow* mainWindow;
    BudgetPlan currentPlan;

    void populateRecentTable();
    void addRecentRow(const class Transaction* tx);
    void setupCardShadows();
    void setupRecentTable();
    void updateCards();
    void updateCalendar();
    void updatePlanCard();
    void updateBudgetProgress();
    void updateGoalProgress();
};
