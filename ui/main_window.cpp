#include "main_window.h"

#include "data_paths.h"
#include "dashboard_page.h"
#include "transaction_page.h"
#include "wallet_page.h"
#include "budget_page.h"
#include "statistics_page.h"
#include "category_page.h"

#include "../storage/file_manager.h"

#include <QButtonGroup>
#include <QCloseEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QString userName,
                       std::function<void()> onLogout,
                       QWidget* parent)
    : QMainWindow(parent),
      userName(std::move(userName)),
      onLogout(std::move(onLogout)) {

    setWindowTitle("MoneyManager");
    setMinimumSize(1080, 700);
    resize(1320, 820);

    loadData();
    initializeDefaults();
    saveAll();
    buildUi();
    refreshAll();
}

void MainWindow::loadData() {
    FileManager::ensureDataDirectory(DataPaths::dataDir().toStdString());

    FileManager::loadCategories(DataPaths::categoryFile(), categories);
    FileManager::loadWallets(DataPaths::walletFile(), wallets);
    FileManager::loadTransactions(DataPaths::transactionFile(), transactions);
    FileManager::loadBudgets(DataPaths::budgetFile(), budgets);
}

void MainWindow::initializeDefaults() {
    if (categories.getCategories().isEmpty()) {
        categories.createCategory("Ăn uống", CategoryType::EXPENSE);
        categories.createCategory("Đi lại", CategoryType::EXPENSE);
        categories.createCategory("Mua sắm", CategoryType::EXPENSE);
        categories.createCategory("Giải trí", CategoryType::EXPENSE);
        categories.createCategory("Học tập", CategoryType::EXPENSE);
        categories.createCategory("Sức khỏe", CategoryType::EXPENSE);
        categories.createCategory("Nhà ở", CategoryType::EXPENSE);
        categories.createCategory("Hóa đơn", CategoryType::EXPENSE);
        categories.createCategory("Khác", CategoryType::EXPENSE);

        categories.createCategory("Lương", CategoryType::INCOME);
        categories.createCategory("Thưởng", CategoryType::INCOME);
        categories.createCategory("Làm thêm", CategoryType::INCOME);
        categories.createCategory("Đầu tư", CategoryType::INCOME);
        categories.createCategory("Thu nhập khác", CategoryType::INCOME);
    }

    if (wallets.getWallets().isEmpty()) {
        wallets.createWallet("Tiền mặt", WalletType::CASH, 0);
    }
}

void MainWindow::saveAll() {
    FileManager::saveCategories(DataPaths::categoryFile(), categories);
    FileManager::saveWallets(DataPaths::walletFile(), wallets);
    FileManager::saveTransactions(DataPaths::transactionFile(), transactions);
    FileManager::saveBudgets(DataPaths::budgetFile(), budgets);
}

void MainWindow::refreshAll() {
    if (dashboardPage) dashboardPage->refresh();
    if (transactionPage) transactionPage->refresh();
    if (walletPage) walletPage->refresh();
    if (budgetPage) budgetPage->refresh();
    if (statisticsPage) statisticsPage->refresh();
    if (categoryPage) categoryPage->refresh();
}

void MainWindow::buildUi() {
    QWidget* central = new QWidget();
    QHBoxLayout* root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    QFrame* sidebar = new QFrame();
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(230);

    QVBoxLayout* sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(18, 22, 18, 22);
    sideLayout->setSpacing(6);

    QLabel* brand = new QLabel("MONEY\nMANAGER");
    brand->setObjectName("brand");

    QLabel* userLabel = new QLabel("Xin chào, " + userName);
    userLabel->setObjectName("sidebarUser");

    sideLayout->addWidget(brand);
    sideLayout->addWidget(userLabel);
    sideLayout->addSpacing(22);

    navigationGroup = new QButtonGroup(this);
    navigationGroup->setExclusive(true);

    QStringList labels = {
        "Tổng quan",
        "Giao dịch",
        "Ví",
        "Ngân sách",
        "Thống kê",
        "Danh mục"
    };

    for (int i = 0; i < labels.size(); ++i) {
        QPushButton* button = new QPushButton(labels[i]);
        button->setObjectName("sidebarButton");
        button->setCheckable(true);
        navigationGroup->addButton(button, i);
        sideLayout->addWidget(button);

        connect(button, &QPushButton::clicked, this, [this, i]() {
            pageStack->setCurrentIndex(i);
        });

        if (i == 0) button->setChecked(true);
    }

    sideLayout->addStretch();

    QPushButton* logoutButton = new QPushButton("Đăng xuất");
    logoutButton->setObjectName("sidebarButton");
    sideLayout->addWidget(logoutButton);

    connect(logoutButton, &QPushButton::clicked, this, [this]() {
        handleLogout();
    });

    pageStack = new QStackedWidget();

    auto changed = [this]() {
        saveAll();
        refreshAll();
    };

    dashboardPage = new DashboardPage(
        &transactions, &wallets, &categories, &budgets
    );
    transactionPage = new TransactionPage(
        &transactions, &wallets, &categories, changed
    );
    walletPage = new WalletPage(
        &wallets, &transactions, changed
    );
    budgetPage = new BudgetPage(
        &budgets, &transactions, &categories, changed
    );
    statisticsPage = new StatisticsPage(
        &transactions, &categories
    );
    categoryPage = new CategoryPage(
        &categories, &transactions, &budgets, changed
    );

    pageStack->addWidget(dashboardPage);
    pageStack->addWidget(transactionPage);
    pageStack->addWidget(walletPage);
    pageStack->addWidget(budgetPage);
    pageStack->addWidget(statisticsPage);
    pageStack->addWidget(categoryPage);

    root->addWidget(sidebar);
    root->addWidget(pageStack, 1);

    setCentralWidget(central);
}

void MainWindow::handleLogout() {
    if (QMessageBox::question(
            this,
            "Đăng xuất",
            "Bạn có chắc muốn đăng xuất?"
        ) != QMessageBox::Yes) {
        return;
    }

    saveAll();

    if (onLogout) onLogout();
    close();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    saveAll();
    QMainWindow::closeEvent(event);
}
