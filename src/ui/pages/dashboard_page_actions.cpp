#include "dashboard_page.h"
#include "ui_dashboard_page.h"
#include "../dialogs/transaction_dialog.h"
#include "../dialogs/day_detail_dialog.h"
#include "../dialogs/plan_dialog.h"
#include "../gui_helpers.h"
#include "../main_window.h"
#include "../../core/app/money_manager.h"
#include <QMessageBox>

int DashboardPage::selectedMonth() const { return ui->monthPicker->month(); }
int DashboardPage::selectedYear() const { return ui->monthPicker->year(); }

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
        currentPlan = dlg.getPlan();
        PlanDialog::savePlanToFile(dataDirectory, currentPlan);
        updatePlanCard();
        if (mainWindow) mainWindow->refreshAllPages();
    }
}
