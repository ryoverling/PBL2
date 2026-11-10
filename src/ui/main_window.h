#pragma once

#include <QMainWindow>
#include <string>
#include "../core/utils/date.h"

class MoneyManager;
class DashboardPage;
class TransactionPage;
class WalletPage;
class StatisticsPage;
class SettingsPage;

namespace Ui {
class MainWindow;
}

// Cửa sổ chính của ứng dụng
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(MoneyManager& mm, const std::string& userDir, const std::string& baseDir, QWidget *parent = nullptr);
    ~MainWindow();

    bool isLoggedOut() const;
    void performLogout();
    void refreshAllPages();
    void showTransactions();
    void showTransactionsWithDate(const Date& date);
    void navigateToPage(int index);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btnNavDashboard_clicked();
    void on_btnNavTransactions_clicked();
    void on_btnNavWallets_clicked();
    void on_btnNavStats_clicked();
    void on_btnNavSettings_clicked();

private:
    Ui::MainWindow *ui;
    MoneyManager& moneyManager;
    std::string userDirectory;
    std::string baseDataDirectory;
    bool loggedOut = false;

    DashboardPage* pageDashboard;
    TransactionPage* pageTransactions;
    WalletPage* pageWallets;
    StatisticsPage* pageStatistics;
    SettingsPage* pageSettings;

    void updateNavButtons(int activeIndex);
    void setupPages();
};
