#include "dashboard_page.h"
#include "ui_dashboard_page.h"
#include "../gui_helpers.h"
#include "../main_window.h"
#include "../dialogs/transaction_dialog.h"
#include "../dialogs/day_detail_dialog.h"
#include "../dialogs/plan_dialog.h"
#include "../../core/app/money_manager.h"
#include <QStyle>

void DashboardPage::setupCardShadows() {
    UiHelper::applyCardShadows({
        ui->cardBalance, ui->cardIncome, ui->cardExpense, ui->cardNet,
        ui->frmCalendarCard, ui->frmPlanCard, ui->frmTableCard
    });
}

void DashboardPage::setupRecentTable() {
    ui->tblRecentTransactions->setShowGrid(false);
    ui->tblRecentTransactions->setIconSize(QSize(28, 28));
    ui->tblRecentTransactions->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tblRecentTransactions->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tblRecentTransactions->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tblRecentTransactions->verticalHeader()->setVisible(false);
    ui->tblRecentTransactions->verticalHeader()->setDefaultSectionSize(48);
}

DashboardPage::DashboardPage(MoneyManager& mm, const std::string& dataDir, MainWindow* mainWin, QWidget *parent)
    : QWidget(parent), ui(new Ui::DashboardPage), moneyManager(mm),
      dataDirectory(dataDir), mainWindow(mainWin) {
    ui->setupUi(this);

    ui->btnAddTransaction->setText(QString::fromUtf8("Thêm giao dịch"));
    ui->btnAddTransaction->setIcon(UiHelper::createPlusIcon());

    ui->btnViewAll->setText(QString::fromUtf8("Xem tất cả →"));
    ui->btnViewAll->setProperty("role", "ghost");

    ui->btnEditPlan->setIcon(UiHelper::createEditIcon(QColor("#1E6FFF")));

    ui->lblIncomeSparkline->setPixmap(UiHelper::createSparkline(true, 56, 26));
    ui->lblExpenseSparkline->setPixmap(UiHelper::createSparkline(false, 56, 26));
    ui->lblBudgetIcon->setPixmap(UiHelper::createPlanBadge("budget", 24));
    ui->lblGoalIcon->setPixmap(UiHelper::createPlanBadge("goal", 24));

    ui->prgBudget->setFixedHeight(12);
    ui->prgGoal->setFixedHeight(12);

    setupCardShadows();
    setupRecentTable();

    Date today = Date::today();
    ui->monthPicker->setMonthYear(today.month, today.year);

    PlanDialog::loadPlanFromFile(dataDirectory, currentPlan);
    refreshData();
}

DashboardPage::~DashboardPage() { delete ui; }

int DashboardPage::selectedMonth() const { return ui->monthPicker->month(); }
int DashboardPage::selectedYear() const { return ui->monthPicker->year(); }

void DashboardPage::addRecentRow(const Transaction* tx) {
    int row = ui->tblRecentTransactions->rowCount();
    ui->tblRecentTransactions->insertRow(row);
    bool income = tx->getType() == TransactionType::INCOME;
    const Category* category = moneyManager.getCategoryManager().findById(tx->getCategoryId());

    QString amountPrefix = income ? "↑ + " : "↓ - ";
    QTableWidgetItem* amount = new QTableWidgetItem(amountPrefix + UiHelper::formatMoneyVND(tx->getAmount()));
    amount->setForeground(income ? UiHelper::colorIncome() : UiHelper::colorExpense());
    QFont f = amount->font();
    f.setBold(true);
    amount->setFont(f);
    amount->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QString catName = category ? QString::fromStdString(category->getName()) : "Khác";
    QString detail = catName;
    if (!tx->getNote().empty()) {
        detail += " • " + QString::fromStdString(tx->getNote());
    }

    QTableWidgetItem* dateItem = new QTableWidgetItem(UiHelper::formatDate(tx->getDate()));
    dateItem->setForeground(QColor("#5B6B8C"));

    QTableWidgetItem* detailItem = new QTableWidgetItem(UiHelper::createCategoryCircleIcon(catName, 28), detail);
    detailItem->setForeground(QColor("#0B2A6B"));
    QFont fDetail = detailItem->font();
    fDetail.setBold(true);
    detailItem->setFont(fDetail);
    detailItem->setToolTip(detail);

    ui->tblRecentTransactions->setItem(row, 0, dateItem);
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
    const char* netProp = "zero";
    if (net > 0) {
        netProp = "positive";
    } else if (net < 0) {
        netProp = "negative";
    }
    ui->lblNetTotal->setProperty("netState", netProp);
    ui->lblNetTotal->style()->unpolish(ui->lblNetTotal);
    ui->lblNetTotal->style()->polish(ui->lblNetTotal);
    ui->lblTotalBalance->setText(UiHelper::formatBalance(moneyManager.getWalletManager().getTotalBalance()));
    ui->btnEye->setChecked(UiHelper::balanceHidden);
}

void DashboardPage::updateCalendar() {
    DailyExpenseList daily = moneyManager.getExpenseByDay(selectedMonth(), selectedYear());
    ui->calDaily->setData(selectedMonth(), selectedYear(), daily.days);
}

void DashboardPage::updateBudgetProgress() {
    if (currentPlan.monthlyBudget <= 0) {
        ui->prgBudget->setValue(0);
        ui->lblBudgetLabel->setText("Ngân sách chi tiêu tháng: Chưa thiết lập");
        return;
    }
    long long income = 0, expense = 0;
    moneyManager.getMonthlyTotals(selectedMonth(), selectedYear(), income, expense);
    double pct = (double)expense / (double)currentPlan.monthlyBudget * 100.0;
    int clampedVal = qBound(0, static_cast<int>(pct), 100);
    ui->prgBudget->setValue(clampedVal);

    const char* variant = "success";
    if (pct >= 100.0) {
        variant = "danger";
    } else if (pct >= 75.0) {
        variant = "warning";
    }
    ui->prgBudget->setProperty("variant", variant);
    ui->prgBudget->style()->unpolish(ui->prgBudget);
    ui->prgBudget->style()->polish(ui->prgBudget);

    ui->lblBudgetLabel->setText(QString("Ngân sách chi tiêu tháng: %1 / %2 (%3%)")
        .arg(UiHelper::formatMoneyVND(expense))
        .arg(UiHelper::formatMoneyVND(currentPlan.monthlyBudget))
        .arg(QString::number(pct, 'f', 1)));
}

void DashboardPage::updateGoalProgress() {
    if (currentPlan.goalName.empty() || currentPlan.targetAmount <= 0) {
        ui->prgGoal->setValue(0);
        ui->lblGoalLabel->setText("Mục tiêu tiết kiệm: Chưa thiết lập");
        return;
    }
    double pct = (double)currentPlan.currentSaved / (double)currentPlan.targetAmount * 100.0;
    int clampedVal = qBound(0, static_cast<int>(pct), 100);
    ui->prgGoal->setValue(clampedVal);
    ui->prgGoal->setProperty("variant", "primary");
    ui->prgGoal->style()->unpolish(ui->prgGoal);
    ui->prgGoal->style()->polish(ui->prgGoal);

    QString goalStr = QString("Mục tiêu \"%1\": %2 / %3 (%4%)")
        .arg(QString::fromStdString(currentPlan.goalName))
        .arg(UiHelper::formatMoneyVND(currentPlan.currentSaved))
        .arg(UiHelper::formatMoneyVND(currentPlan.targetAmount))
        .arg(QString::number(pct, 'f', 1));
    if (!currentPlan.targetDate.empty()) {
        goalStr += " (Hạn: " + QString::fromStdString(currentPlan.targetDate) + ")";
    }
    ui->lblGoalLabel->setText(goalStr);
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

    updateBudgetProgress();
    updateGoalProgress();
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

void DashboardPage::on_btnAddTransaction_clicked() {
    TransactionDialog dialog(moneyManager, -1, this);
    if (dialog.exec() != QDialog::Accepted) return;
    int id = moneyManager.getTransactionManager().getNextId();
    Transaction* tx = dialog.createTransaction(id, 0);
    CategoryType type = dialog.getType() == TransactionType::INCOME ? CategoryType::INCOME : CategoryType::EXPENSE;
    if (!moneyManager.addTransaction(tx, dialog.getCategoryName(), type)) {
        delete tx;
        UiHelper::showError(this, "Không thể thêm giao dịch.");
        return;
    }
    bool saved = moneyManager.saveAll(dataDirectory);
    if (!saved) UiHelper::showError(this, "Không lưu được dữ liệu.");
    if (mainWindow) mainWindow->refreshAllPages(); else refreshData();
    if (saved) UiHelper::showInfo(this, "Đã lưu giao dịch.");
}

void DashboardPage::on_calDaily_dayClicked(int day) {
    Date target(day, selectedMonth(), selectedYear());
    DayDetailDialog dlg(moneyManager, target, mainWindow, this);
    dlg.exec();
}

void DashboardPage::on_btnEditPlan_clicked() {
    PlanDialog dlg(currentPlan, this);
    if (dlg.exec() == QDialog::Accepted) {
        BudgetPlan newPlan = dlg.getPlan();
        if (!PlanDialog::savePlanToFile(dataDirectory, newPlan)) {
            UiHelper::showError(this, "Không thể lưu kế hoạch tài chính vào tệp.");
            return;
        }
        currentPlan = newPlan;
        updatePlanCard();
        if (mainWindow) mainWindow->refreshAllPages();
    }
}
