#include "dashboard_page.h"
#include "ui_dashboard_page.h"
#include "../gui_helpers.h"
#include "../main_window.h"
#include "../../core/app/money_manager.h"
#include <QStyle>

DashboardPage::DashboardPage(MoneyManager& mm, const std::string& dataDir, MainWindow* mainWin, QWidget *parent)
    : QWidget(parent), ui(new Ui::DashboardPage), moneyManager(mm),
      dataDirectory(dataDir), mainWindow(mainWin) {
    ui->setupUi(this);

    ui->btnAddTransaction->setText(QString::fromUtf8("Thêm giao dịch"));
    ui->btnAddTransaction->setIcon(UiHelper::createPlusIcon());
    ui->btnViewAll->setIcon(UiHelper::createListIcon(QColor("#1A6BEB")));

    ui->tblRecentTransactions->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblRecentTransactions->verticalHeader()->setVisible(false);

    Date today = Date::today();
    ui->monthPicker->setMonthYear(today.month, today.year);
    connect(ui->monthPicker, &MonthPickerWidget::monthYearChanged, this, &DashboardPage::on_monthPicker_monthYearChanged);
    connect(ui->btnEditPlan, &QPushButton::clicked, this, &DashboardPage::on_btnEditPlan_clicked);

    PlanDialog::loadPlanFromFile(dataDirectory, currentPlan);
    refreshData();
}

DashboardPage::~DashboardPage() { delete ui; }

void DashboardPage::addRecentRow(const Transaction* tx) {
    int row = ui->tblRecentTransactions->rowCount();
    ui->tblRecentTransactions->insertRow(row);
    bool income = tx->getType() == TransactionType::INCOME;
    const Category* category = moneyManager.getCategoryManager().findById(tx->getCategoryId());

    QString amountPrefix = income ? "↑ + " : "↓ - ";
    auto* amount = new QTableWidgetItem(amountPrefix + UiHelper::formatMoneyVND(tx->getAmount()));
    amount->setForeground(QColor(income ? "#10B981" : "#EF4444"));

    QString catName = category ? QString::fromStdString(category->getName()) : "Khác";
    QString detail = UiHelper::getCategoryIcon(catName) + " " + catName;
    if (!tx->getNote().empty()) {
        detail += " • " + QString::fromStdString(tx->getNote());
    }

    amount->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tblRecentTransactions->setItem(row, 0, new QTableWidgetItem(UiHelper::formatDate(tx->getDate())));
    auto* detailItem = new QTableWidgetItem(detail);
    detailItem->setToolTip(detail);
    ui->tblRecentTransactions->setItem(row, 1, detailItem);
    ui->tblRecentTransactions->setItem(row, 2, amount);
}

void DashboardPage::populateRecentTable() {
    ui->tblRecentTransactions->setRowCount(0);
    ui->tblRecentTransactions->clearSpans();
    LinkedList<Transaction*> list;
    moneyManager.getFilteredTransactions(list);
    for (const Node<Transaction*>* cur = list.getHead(); cur; cur = cur->next) {
        if (ui->tblRecentTransactions->rowCount() == 5) break;
        if (cur->data) addRecentRow(cur->data);
    }
    if (list.isEmpty()) {
        ui->tblRecentTransactions->setRowCount(1);
        ui->tblRecentTransactions->setSpan(0, 0, 1, 3);
        ui->tblRecentTransactions->setItem(0, 0, new QTableWidgetItem("Chưa có giao dịch gần đây"));
    }
}

void DashboardPage::updateCards() {
    int month = selectedMonth(), year = selectedYear();
    long long income = 0, expense = 0;
    moneyManager.getMonthlyTotals(month, year, income, expense);
    ui->lblTotalIncome->setText(UiHelper::formatMoneyVND(income));
    ui->lblTotalExpense->setText(UiHelper::formatMoneyVND(expense));
    long long net = moneyManager.getMonthlyDifference(month, year);
    ui->lblNetTotal->setText(UiHelper::formatMoneyVND(net));
    ui->lblNetTotal->setProperty("netState", net > 0 ? "positive" : net < 0 ? "negative" : "zero");
    ui->lblNetTotal->style()->unpolish(ui->lblNetTotal);
    ui->lblNetTotal->style()->polish(ui->lblNetTotal);
    ui->lblTotalBalance->setText(UiHelper::formatBalance(moneyManager.getWalletManager().getTotalBalance()));
    ui->btnEye->setChecked(UiHelper::balanceHidden);
}

void DashboardPage::updateCalendar() {
    DailyExpenseList daily = moneyManager.getExpenseByDay(selectedMonth(), selectedYear());
    ui->calDaily->setData(selectedMonth(), selectedYear(), daily.days);
}

void DashboardPage::updatePlanCard() {
    if (!currentPlan.hasPlan()) {
        ui->widgetPlanData->setVisible(false);
        ui->lblEmptyPlan->setVisible(true);
        ui->btnEditPlan->setText("+ Thêm kế hoạch");
        return;
    }

    ui->widgetPlanData->setVisible(true);
    ui->lblEmptyPlan->setVisible(false);
    ui->btnEditPlan->setText("Thiết lập");

    // 1. Ngân sách chi tiêu tháng
    if (currentPlan.monthlyBudget > 0) {
        long long income = 0, expense = 0;
        moneyManager.getMonthlyTotals(selectedMonth(), selectedYear(), income, expense);
        double pct = (double)expense / (double)currentPlan.monthlyBudget * 100.0;
        int clampedVal = qBound(0, static_cast<int>(pct), 100);
        ui->prgBudget->setValue(clampedVal);

        QString statusColor = "#10B981"; // Xanh ngọc an toàn
        if (pct >= 100.0) {
            statusColor = "#EF4444"; // Đỏ cảnh báo vượt ngân sách
        } else if (pct >= 75.0) {
            statusColor = "#F59E0B"; // Cam gần đạt ngưỡng
        }
        ui->prgBudget->setStyleSheet(QString(
            "QProgressBar { background-color: #E2E8F0; border-radius: 4px; } "
            "QProgressBar::chunk { background-color: %1; border-radius: 4px; }"
        ).arg(statusColor));

        ui->lblBudgetLabel->setText(QString("Ngân sách chi tiêu tháng: %1 / %2 (%3%)")
            .arg(UiHelper::formatMoneyVND(expense))
            .arg(UiHelper::formatMoneyVND(currentPlan.monthlyBudget))
            .arg(QString::number(pct, 'f', 1)));
    } else {
        ui->prgBudget->setValue(0);
        ui->lblBudgetLabel->setText("Ngân sách chi tiêu tháng: Chưa thiết lập");
    }

    // 2. Mục tiêu tiết kiệm
    if (!currentPlan.goalName.empty() && currentPlan.targetAmount > 0) {
        double pct = (double)currentPlan.currentSaved / (double)currentPlan.targetAmount * 100.0;
        int clampedVal = qBound(0, static_cast<int>(pct), 100);
        ui->prgGoal->setValue(clampedVal);
        ui->prgGoal->setStyleSheet(
            "QProgressBar { background-color: #E2E8F0; border-radius: 4px; } "
            "QProgressBar::chunk { background-color: #1A6BEB; border-radius: 4px; }"
        );

        QString goalStr = QString("Mục tiêu \"%1\": %2 / %3 (%4%)")
            .arg(QString::fromStdString(currentPlan.goalName))
            .arg(UiHelper::formatMoneyVND(currentPlan.currentSaved))
            .arg(UiHelper::formatMoneyVND(currentPlan.targetAmount))
            .arg(QString::number(pct, 'f', 1));
        if (!currentPlan.targetDate.empty()) {
            goalStr += " (Hạn: " + QString::fromStdString(currentPlan.targetDate) + ")";
        }
        ui->lblGoalLabel->setText(goalStr);
    } else {
        ui->prgGoal->setValue(0);
        ui->lblGoalLabel->setText("Mục tiêu tiết kiệm: Chưa thiết lập");
    }
}

void DashboardPage::refreshData() {
    updateCards();
    updateCalendar();
    updatePlanCard();
    populateRecentTable();
}

void DashboardPage::on_monthPicker_monthYearChanged(int, int) {
    refreshData();
}

void DashboardPage::on_btnEye_clicked() {
    UiHelper::balanceHidden = ui->btnEye->isChecked();
    if (mainWindow) mainWindow->refreshAllPages();
    else refreshData();
}

void DashboardPage::on_btnViewAll_clicked() {
    if (mainWindow) mainWindow->showTransactions();
}
