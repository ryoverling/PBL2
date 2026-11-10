#include "main_window.h"
#include "ui_main_window.h"
#include "gui_helpers.h"
#include "../core/app/money_manager.h"
#include "pages/dashboard_page.h"
#include "pages/transaction_page.h"
#include "pages/wallet_page.h"
#include "pages/statistics_page.h"
#include "pages/settings_page.h"
#include <QApplication>
#include <QCloseEvent>
#include <QDate>

MainWindow::MainWindow(MoneyManager& mm, const std::string& userDir, const std::string& baseDir, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), moneyManager(mm),
      userDirectory(userDir), baseDataDirectory(baseDir) {
    ui->setupUi(this);
    UiHelper::setupWindowTheme(this);

    // Thiet lap Logo App va kich thuoc icon nav
    ui->lblAppLogo->setPixmap(UiHelper::createAppLogoIcon(32).pixmap(32, 32));
    QSize navIconSize(18, 18);
    ui->btnNavDashboard->setIconSize(navIconSize);
    ui->btnNavTransactions->setIconSize(navIconSize);
    ui->btnNavWallets->setIconSize(navIconSize);
    ui->btnNavStats->setIconSize(navIconSize);
    ui->btnNavSettings->setIconSize(navIconSize);

    setupPages();
    updateNavButtons(0);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupPages() {
    pageDashboard = new DashboardPage(moneyManager, userDirectory, this, this);
    pageTransactions = new TransactionPage(moneyManager, userDirectory, this, this);
    pageWallets = new WalletPage(moneyManager, userDirectory, this, this);
    pageStatistics = new StatisticsPage(moneyManager, this);
    pageSettings = new SettingsPage(moneyManager, userDirectory, baseDataDirectory, this, this);

    ui->stackedWidget->addWidget(pageDashboard);
    ui->stackedWidget->addWidget(pageTransactions);
    ui->stackedWidget->addWidget(pageWallets);
    ui->stackedWidget->addWidget(pageStatistics);
    ui->stackedWidget->addWidget(pageSettings);
}

void MainWindow::updateNavButtons(int activeIndex) {
    QColor activeColor("#1E6FFF");
    QColor inactiveColor("#64748B");

    ui->btnNavDashboard->setChecked(activeIndex == 0);
    ui->btnNavDashboard->setIcon(UiHelper::createHomeNavIcon(activeIndex == 0 ? activeColor : inactiveColor));

    ui->btnNavTransactions->setChecked(activeIndex == 1);
    ui->btnNavTransactions->setIcon(UiHelper::createExchangeNavIcon(activeIndex == 1 ? activeColor : inactiveColor));

    ui->btnNavWallets->setChecked(activeIndex == 2);
    ui->btnNavWallets->setIcon(UiHelper::createWalletNavIcon(activeIndex == 2 ? activeColor : inactiveColor));

    ui->btnNavStats->setChecked(activeIndex == 3);
    ui->btnNavStats->setIcon(UiHelper::createStatsNavIcon(activeIndex == 3 ? activeColor : inactiveColor));

    ui->btnNavSettings->setChecked(activeIndex == 4);
    ui->btnNavSettings->setIcon(UiHelper::createSettingsNavIcon(activeIndex == 4 ? activeColor : inactiveColor));
}

void MainWindow::on_btnNavDashboard_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
    updateNavButtons(0);
    pageDashboard->refreshData();
}

void MainWindow::on_btnNavTransactions_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
    updateNavButtons(1);
    pageTransactions->refreshData();
}

void MainWindow::on_btnNavWallets_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    updateNavButtons(2);
    pageWallets->refreshData();
}

void MainWindow::on_btnNavStats_clicked() {
    ui->stackedWidget->setCurrentIndex(3);
    updateNavButtons(3);
    pageStatistics->refreshData();
}

void MainWindow::on_btnNavSettings_clicked() {
    ui->stackedWidget->setCurrentIndex(4);
    updateNavButtons(4);
}

void MainWindow::navigateToPage(int index) {
    if (index == 0) on_btnNavDashboard_clicked();
    else if (index == 1) on_btnNavTransactions_clicked();
    else if (index == 2) on_btnNavWallets_clicked();
    else if (index == 3) on_btnNavStats_clicked();
    else if (index == 4) on_btnNavSettings_clicked();
}

void MainWindow::refreshAllPages() {
    pageDashboard->refreshData();
    pageTransactions->refreshData();
    pageWallets->refreshData();
    pageStatistics->refreshData();
}

void MainWindow::showTransactions() {
    ui->stackedWidget->setCurrentIndex(1);
    updateNavButtons(1);
    if (pageTransactions) {
        pageTransactions->refreshData();
    }
}

void MainWindow::showTransactionsWithDate(const Date& date) {
    ui->stackedWidget->setCurrentIndex(1);
    updateNavButtons(1);
    if (pageTransactions) {
        pageTransactions->filterByDate(QDate(date.year, date.month, date.day));
    }
}

bool MainWindow::isLoggedOut() const {
    return loggedOut;
}

void MainWindow::performLogout() {
    loggedOut = true;
    close();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (!loggedOut) QApplication::quit();
    event->accept();
}
