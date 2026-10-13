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
#include <QFileInfo>
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

    dataReady = loadData();
    if (!dataReady) return;
    buildUi();
    refreshAll();
    if (unsavedChanges) saveAll();
}

bool MainWindow::loadData() {
    auto failed = [this]() {
        QMessageBox::critical(this, "Không thể đọc dữ liệu",
            "Dữ liệu bị thiếu, không hợp lệ hoặc không thể truy cập. Các file hiện có được giữ nguyên.\n"
            "Hãy kiểm tra thư mục: " + DataPaths::dataDir());
        return false;
    };
    if (!FileManager::ensureDataDirectory(DataPaths::dataDir().toStdString())) return failed();

    const string paths[] = {DataPaths::categoryFile(), DataPaths::walletFile(),
                            DataPaths::transactionFile(), DataPaths::budgetFile()};
    int existing = 0;
    for (const string& path : paths) {
        if (QFileInfo::exists(QString::fromStdString(path + ".bak"))) return failed();
        if (QFileInfo::exists(QString::fromStdString(path))) ++existing;
    }
    if (existing == 0) {
        initializeDefaults();
        unsavedChanges = true;
        return true;
    }
    if (existing != 4 ||
        !FileManager::loadCategories(DataPaths::categoryFile(), categories) ||
        !FileManager::loadWallets(DataPaths::walletFile(), wallets) ||
        !FileManager::loadTransactions(DataPaths::transactionFile(), transactions) ||
        !FileManager::loadBudgets(DataPaths::budgetFile(), budgets) ||
        !transactions.hasValidReferences(wallets, categories) ||
        !budgets.hasValidReferences(categories)) return failed();
    return true;
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

bool MainWindow::saveAll() {
    if (!dataReady) return false;
    bool saved = FileManager::saveCategories(DataPaths::categoryFile(), categories);
    saved = FileManager::saveWallets(DataPaths::walletFile(), wallets) && saved;
    saved = FileManager::saveTransactions(DataPaths::transactionFile(), transactions) && saved;
    saved = FileManager::saveBudgets(DataPaths::budgetFile(), budgets) && saved;
    unsavedChanges = !saved;
    if (!saved) {
        QMessageBox::warning(this, "Chưa lưu được dữ liệu",
            "Không thể lưu đầy đủ thay đổi. Hãy kiểm tra dung lượng và quyền ghi thư mục:\n" +
            DataPaths::dataDir() + "\nGiữ ứng dụng mở và thử lại sau khi khắc phục.");
    }
    return saved;
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
        unsavedChanges = true;
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

    loggingOut = true;
    close();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (dataReady && unsavedChanges && !saveAll()) {
        loggingOut = false;
        event->ignore();
        return;
    }
    if (loggingOut && onLogout) onLogout();
    QMainWindow::closeEvent(event);
}
