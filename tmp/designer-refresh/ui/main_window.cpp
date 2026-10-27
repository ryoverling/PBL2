#include "main_window.h"
#include "ui/ui_main_window.h"

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
      ui(new Ui::MainWindow),
      userName(std::move(userName)),
      onLogout(std::move(onLogout)) {

    dataReady = loadData();
    if (!dataReady) return;
    ui->setupUi(this);
    buildUi();
    refreshAll();
    if (unsavedChanges) saveAll();
}

MainWindow::~MainWindow() {
    delete ui;
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
    ui->userLabel->setText("Xin chào, " + userName);
    pageStack = ui->pageStack;

    navigationGroup = new QButtonGroup(this);
    navigationGroup->setExclusive(true);
    const QList<QPushButton*> buttons = {
        ui->dashboardButton, ui->transactionButton, ui->walletButton,
        ui->budgetButton, ui->statisticsButton, ui->categoryButton
    };
    for (int i = 0; i < buttons.size(); ++i) {
        QPushButton* button = buttons[i];
        navigationGroup->addButton(button, i);
        connect(button, &QPushButton::clicked, this, [this, i]() {
            pageStack->setCurrentIndex(i);
        });
    }
    connect(ui->logoutButton, &QPushButton::clicked, this, [this]() {
        handleLogout();
    });

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
