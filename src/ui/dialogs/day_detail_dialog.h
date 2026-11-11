#pragma once

#include <QDialog>
#include "../../core/utils/utils.h"

class MoneyManager;
class MainWindow;
class Transaction;

namespace Ui {
class DayDetailDialog;
}

class DayDetailDialog : public QDialog {
    Q_OBJECT

public:
    explicit DayDetailDialog(MoneyManager& mm, const Date& date, MainWindow* mainWin = nullptr, QWidget* parent = nullptr);
    ~DayDetailDialog() override;

private slots:
    void on_btnClose_clicked();
    void on_btnViewInTransactions_clicked();

private:
    Ui::DayDetailDialog *ui;
    MoneyManager& moneyManager;
    Date targetDate;
    MainWindow* mainWindow;

    void populateData();
    void setupKpiBadges();
    QWidget* createTypeBadgeWidget(bool isIncome);
    void addDayRow(const Transaction* tx, int row);
    void updateSummaryTotals(long long income, long long expense);
};

