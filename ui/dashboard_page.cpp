#include "dashboard_page.h"

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


namespace {
    QFrame* makeMetricCard(const QString& caption, QLabel*& valueLabel) {
        QFrame* card = new QFrame();
        card->setObjectName("card");

        QVBoxLayout* layout = new QVBoxLayout(card);
        layout->setContentsMargins(18, 16, 18, 16);

        QLabel* captionLabel = new QLabel(caption);
        captionLabel->setObjectName("mutedLabel");

        valueLabel = new QLabel("0 đ");
        valueLabel->setStyleSheet("font-size: 23px; font-weight: 800;");

        layout->addWidget(captionLabel);
        layout->addWidget(valueLabel);
        return card;
    }
}

DashboardPage::DashboardPage(TransactionManager* transactions,
                             WalletManager* wallets,
                             CategoryManager* categories,
                             BudgetManager* budgets,
                             QWidget* parent)
    : QWidget(parent),
      transactions(transactions),
      wallets(wallets),
      categories(categories),
      budgets(budgets) {

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 28);
    root->setSpacing(16);

    QHBoxLayout* header = new QHBoxLayout();
    QLabel* title = new QLabel("Tổng quan");
    title->setObjectName("pageTitle");
    QLabel* date = new QLabel(QDate::currentDate().toString("'Hôm nay,' dd/MM/yyyy"));
    date->setObjectName("mutedLabel");
    header->addWidget(title);
    header->addStretch();
    header->addWidget(date);

    QGridLayout* metrics = new QGridLayout();
    metrics->setHorizontalSpacing(14);
    metrics->addWidget(makeMetricCard("Tổng tài sản", totalBalanceLabel), 0, 0);
    metrics->addWidget(makeMetricCard("Thu tháng này", incomeLabel), 0, 1);
    metrics->addWidget(makeMetricCard("Chi tháng này", expenseLabel), 0, 2);
    metrics->addWidget(makeMetricCard("Chênh lệch", differenceLabel), 0, 3);

    QGridLayout* middle = new QGridLayout();
    middle->setHorizontalSpacing(14);

    QFrame* chartCard = new QFrame();
    chartCard->setObjectName("card");
    QVBoxLayout* chartLayout = new QVBoxLayout(chartCard);
    chartLayout->setContentsMargins(18, 16, 18, 16);
    QLabel* chartTitle = new QLabel("Chi tiêu tháng này");
    chartTitle->setObjectName("sectionTitle");
    donutChart = new DonutChartWidget();
    chartLayout->addWidget(chartTitle);
    chartLayout->addWidget(donutChart);

    QFrame* budgetCard = new QFrame();
    budgetCard->setObjectName("card");
    QVBoxLayout* budgetLayout = new QVBoxLayout(budgetCard);
    budgetLayout->setContentsMargins(18, 16, 18, 16);
    QLabel* budgetTitle = new QLabel("Ngân sách đang áp dụng");
    budgetTitle->setObjectName("sectionTitle");
    budgetDetailLabel = new QLabel("Chưa có ngân sách đang áp dụng.");
    budgetDetailLabel->setWordWrap(true);
    budgetDetailLabel->setObjectName("mutedLabel");
    budgetProgress = new QProgressBar();
    budgetProgress->setRange(0, 100);
    budgetProgress->setValue(0);
    budgetLayout->addWidget(budgetTitle);
    budgetLayout->addSpacing(12);
    budgetLayout->addWidget(budgetDetailLabel);
    budgetLayout->addSpacing(8);
    budgetLayout->addWidget(budgetProgress);
    budgetLayout->addStretch();

    middle->addWidget(chartCard, 0, 0);
    middle->addWidget(budgetCard, 0, 1);
    middle->setColumnStretch(0, 2);
    middle->setColumnStretch(1, 1);

    QFrame* recentCard = new QFrame();
    recentCard->setObjectName("card");
    QVBoxLayout* recentLayout = new QVBoxLayout(recentCard);
    recentLayout->setContentsMargins(18, 16, 18, 16);
    QLabel* recentTitle = new QLabel("Giao dịch gần đây");
    recentTitle->setObjectName("sectionTitle");

    recentTable = new QTableWidget(0, 5);
    recentTable->setHorizontalHeaderLabels({"Thời gian", "Loại", "Danh mục", "Ví", "Số tiền"});
    recentTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    recentTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    recentTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    recentTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    recentTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    recentTable->verticalHeader()->setVisible(false);
    recentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    recentTable->setSelectionMode(QAbstractItemView::NoSelection);
    recentTable->setMaximumHeight(245);

    recentLayout->addWidget(recentTitle);
    recentLayout->addWidget(recentTable);

    root->addLayout(header);
    root->addLayout(metrics);
    root->addLayout(middle);
    root->addWidget(recentCard);

    refresh();
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
        if (recentTable->rowCount() >= 6) return;
        const Category* category = categories->findById(t->getCategoryId());
        const Wallet* wallet = wallets->findById(t->getWalletId());

        int row = recentTable->rowCount();
        recentTable->insertRow(row);

        recentTable->setItem(row, 0, new QTableWidgetItem(
            UiHelpers::dateTimeDisplay(t->getDateTime())
        ));
        recentTable->setItem(row, 1, new QTableWidgetItem(
            t->getType() == TransactionType::INCOME ? "Thu nhập" : "Chi tiêu"
        ));
        recentTable->setItem(row, 2, new QTableWidgetItem(
            category ? QString::fromStdString(category->getName()) : "Không rõ"
        ));
        recentTable->setItem(row, 3, new QTableWidgetItem(
            wallet ? QString::fromStdString(wallet->getName()) : "Không rõ"
        ));
        recentTable->setItem(row, 4, new QTableWidgetItem(
            UiHelpers::money(t->getSignedAmount())
        ));
    });
}
