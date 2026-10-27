#include "budget_page.h"
#include "ui/ui_budget_page.h"

#include "budget_dialog.h"
#include "ui_helpers.h"

#include "../manager/budget_manager.h"
#include "../manager/transaction_manager.h"
#include "../manager/category_manager.h"

#include <QAbstractItemView>
#include <QColor>
#include <QDate>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

BudgetPage::BudgetPage(BudgetManager* budgets,
                       TransactionManager* transactions,
                       CategoryManager* categories,
                       std::function<void()> onChanged,
                       QWidget* parent)
    : QWidget(parent),
      ui(new Ui::BudgetPage),
      budgets(budgets),
      transactions(transactions),
      categories(categories),
      onChanged(std::move(onChanged)) {
    ui->setupUi(this);
    table = ui->table;

    connect(ui->addButton, &QPushButton::clicked, this, [this]() { addBudget(); });
    connect(ui->editButton, &QPushButton::clicked, this, [this]() { editBudget(); });
    connect(ui->deleteButton, &QPushButton::clicked, this, [this]() { deleteBudget(); });
    connect(table, &QTableWidget::cellDoubleClicked, this, [this](int, int) { editBudget(); });

    refresh();
}

BudgetPage::~BudgetPage() {
    delete ui;
}

int BudgetPage::selectedId() const {
    int row = table->currentRow();
    if (row < 0 || table->item(row, 0) == nullptr) return -1;
    return table->item(row, 0)->data(Qt::UserRole).toInt();
}

QString BudgetPage::periodLabel(const std::string& start, const std::string& end) const {
    QDate s = QDate::fromString(QString::fromStdString(start), "yyyy-MM-dd");
    QDate e = QDate::fromString(QString::fromStdString(end), "yyyy-MM-dd");

    if (!s.isValid() || !e.isValid()) return "Tùy chọn";
    if (s.daysTo(e) == 6 && s.dayOfWeek() == 1) return "Tuần";
    if (s.day() == 1 && e == s.addMonths(1).addDays(-1)) return "Tháng";
    if (s.month() == 1 && s.day() == 1 && e.month() == 12 && e.day() == 31 &&
        s.year() == e.year()) return "Năm";
    return "Tùy chọn";
}

void BudgetPage::refresh() {
    table->setRowCount(0);

    budgets->getBudgets().forEach([&](const Budget& budget) {
        double spent = budgets->calculateSpent(budget, *transactions);
        double remaining = budgets->calculateRemaining(budget, *transactions);
        bool over = budgets->isOverBudget(budget, *transactions);

        const Category* category = budget.getCategoryId() == 0
            ? nullptr
            : categories->findById(budget.getCategoryId());

        QString categoryName = budget.getCategoryId() == 0
            ? "Tất cả chi tiêu"
            : (category ? QString::fromStdString(category->getName()) : "Không rõ");

        int row = table->rowCount();
        table->insertRow(row);

        QTableWidgetItem* first = new QTableWidgetItem(categoryName);
        first->setData(Qt::UserRole, budget.getId());

        table->setItem(row, 0, first);
        table->setItem(row, 1, new QTableWidgetItem(
            periodLabel(budget.getStartDate(), budget.getEndDate())
        ));
        table->setItem(row, 2, new QTableWidgetItem(
            UiHelpers::dateDisplay(budget.getStartDate()) + " - " +
            UiHelpers::dateDisplay(budget.getEndDate())
        ));
        table->setItem(row, 3, new QTableWidgetItem(UiHelpers::money(budget.getAmount())));
        table->setItem(row, 4, new QTableWidgetItem(UiHelpers::money(spent)));
        table->setItem(row, 5, new QTableWidgetItem(UiHelpers::money(remaining)));
        table->setItem(row, 6, new QTableWidgetItem(over ? "Vượt ngân sách" : "Trong giới hạn"));

        if (over) {
            table->item(row, 6)->setForeground(QColor("#b42318"));
        }
    });
}

void BudgetPage::addBudget() {
    BudgetDialog dialog(categories, nullptr, this);
    if (dialog.exec() != QDialog::Accepted) return;

    int id = budgets->createBudget(
        dialog.amount(),
        dialog.startDate().toStdString(),
        dialog.endDate().toStdString(),
        dialog.categoryId(), *categories
    );

    if (id < 0) {
        QMessageBox::warning(this, "Không thể thêm", "Dữ liệu ngân sách không hợp lệ.");
        return;
    }

    if (onChanged) onChanged();
}

void BudgetPage::editBudget() {
    int id = selectedId();
    Budget* budget = budgets->findById(id);
    if (budget == nullptr) {
        QMessageBox::information(this, "Ngân sách", "Vui lòng chọn một ngân sách.");
        return;
    }

    BudgetDialog dialog(categories, budget, this);
    if (dialog.exec() != QDialog::Accepted) return;

    if (!budgets->updateBudget(id, dialog.amount(), dialog.startDate().toStdString(),
                              dialog.endDate().toStdString(), dialog.categoryId(), *categories)) {
        QMessageBox::warning(this, "Không thể sửa", "Dữ liệu ngân sách không hợp lệ.");
        return;
    }

    if (onChanged) onChanged();
}

void BudgetPage::deleteBudget() {
    int id = selectedId();
    if (budgets->findById(id) == nullptr) {
        QMessageBox::information(this, "Ngân sách", "Vui lòng chọn một ngân sách.");
        return;
    }

    if (QMessageBox::question(
            this,
            "Xóa ngân sách",
            "Bạn có chắc muốn xóa ngân sách đã chọn?"
        ) != QMessageBox::Yes) {
        return;
    }

    budgets->deleteBudget(id);
    if (onChanged) onChanged();
}
