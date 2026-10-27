#include "dashboard_page.h"
#include "ui/ui_dashboard_page.h"

#include "chart_widgets.h"
#include "ui_helpers.h"

#include "../manager/transaction_manager.h"
#include "../manager/wallet_manager.h"
#include "../manager/category_manager.h"
#include "../manager/budget_manager.h"

#include <QAbstractItemView>
#include <QDate>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QProgressBar>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>


DashboardPage::DashboardPage(TransactionManager* transactions,
                             WalletManager* wallets,
                             CategoryManager* categories,
                             BudgetManager* budgets,
                             QWidget* parent)
    : QWidget(parent),
      ui(new Ui::DashboardPage),
      transactions(transactions),
      wallets(wallets),
      categories(categories),
      budgets(budgets) {
    ui->setupUi(this);
    ui->dateLabel->setText(QDate::currentDate().toString("'Hôm nay,' dd/MM/yyyy"));
    totalBalanceLabel = ui->totalBalanceLabel;
    incomeLabel = ui->incomeLabel;
    expenseLabel = ui->expenseLabel;
    differenceLabel = ui->differenceLabel;
    budgetDetailLabel = ui->budgetDetailLabel;
    budgetProgress = ui->budgetProgress;
    recentTable = ui->recentTable;
    UiHelpers::prepareTable(recentTable);
    donutChart = ui->donutChart;
    refresh();
}

DashboardPage::~DashboardPage() {
    delete ui;
}

void DashboardPage::refresh() {
    QDate now = QDate::currentDate();
    QDate first(now.year(), now.month(), 1);
    QDate last = first.addMonths(1).addDays(-1);

    string monthStart = first.toString("yyyy-MM-dd").toStdString();
    string monthEnd = last.toString("yyyy-MM-dd").toStdString();

    double income = transactions->getTotalIncome(monthStart, monthEnd);
    double expense = transactions->getTotalExpense(monthStart, monthEnd);

    totalBalanceLabel->setText(UiHelpers::money(wallets->getTotalBalance()));
    incomeLabel->setText(UiHelpers::money(income));
    expenseLabel->setText(UiHelpers::money(expense));
    differenceLabel->setText(UiHelpers::money(income - expense));

    QVector<QColor> palette = {
        QColor("#6c4ed9"), QColor("#f09a58"), QColor("#51a8a2"),
        QColor("#d56486"), QColor("#5c85d6"), QColor("#d1a83d"),
        QColor("#8d6ab8"), QColor("#7c9a50")
    };

    QVector<ChartSlice> slices;
    int colorIndex = 0;
    categories->getCategories().forEach([&](const Category& category) {
        double value = transactions->getCategoryExpense(category.getId(), monthStart, monthEnd);
        if (value <= 0) return;
        slices.push_back({
            QString::fromStdString(category.getName()),
            value,
            palette[colorIndex % palette.size()]
        });
        colorIndex++;
    });
    donutChart->setData(slices);

    string today = now.toString("yyyy-MM-dd").toStdString();
    double totalBudget = 0;
    double totalSpent = 0;
    int activeCount = 0;

    budgets->getBudgets().forEach([&](const Budget& budget) {
        if (today < budget.getStartDate() || today > budget.getEndDate()) return;
        totalBudget += budget.getAmount();
        totalSpent += budgets->calculateSpent(budget, *transactions);
        activeCount++;
    });

    if (activeCount == 0 || totalBudget <= 0) {
        budgetDetailLabel->setText("Chưa có ngân sách đang áp dụng.");
        budgetProgress->setValue(0);
        budgetProgress->setFormat("0%");
    } else {
        int percent = qRound(totalSpent * 100.0 / totalBudget);
        budgetDetailLabel->setText(
            QString("%1 ngân sách • Đã chi %2 / %3")
                .arg(activeCount)
                .arg(UiHelpers::money(totalSpent))
                .arg(UiHelpers::money(totalBudget))
        );
        budgetProgress->setValue(qMin(percent, 100));
        budgetProgress->setFormat(QString::number(percent) + "%");
    }

    LinkedList<const Transaction*> items;
    transactions->getTransactionsByDate(items);

    recentTable->setRowCount(0);
    items.forEach([&](const Transaction* t) {
        const QDate date = QDate::fromString(QString::fromStdString(t->getDateTime()).left(10), "yyyy-MM-dd");
        if (t->getType() != TransactionType::EXPENSE || date < first || date > last) return;
        const Category* category = categories->findById(t->getCategoryId());
        const Wallet* wallet = wallets->findById(t->getWalletId());

        int row = recentTable->rowCount();
        recentTable->insertRow(row);

        recentTable->setItem(row, 0, new QTableWidgetItem(
            UiHelpers::dateTimeDisplay(t->getDateTime())
        ));
        recentTable->setItem(row, 1, new QTableWidgetItem(
            t->getNote().empty()
                ? (category ? QString::fromStdString(category->getName()) : "Khoản chi")
                : QString::fromStdString(t->getNote())
        ));
        recentTable->setItem(row, 2, new QTableWidgetItem(
            category ? QString::fromStdString(category->getName()) : "Không rõ"
        ));
        recentTable->setItem(row, 3, new QTableWidgetItem(
            wallet ? QString::fromStdString(wallet->getName()) : "Không rõ"
        ));
        recentTable->setItem(row, 4, new QTableWidgetItem(
            UiHelpers::money(t->getAmount())
        ));
    });
    UiHelpers::completeTable(recentTable, {4});
    ui->expenseHint->setText(recentTable->rowCount() == 0
        ? "Chưa có khoản chi trong tháng này. Thêm giao dịch để bắt đầu theo dõi."
        : QString("%1 khoản chi trong tháng • Tổng cộng %2").arg(recentTable->rowCount()).arg(UiHelpers::money(expense)));
}
