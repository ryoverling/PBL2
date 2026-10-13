#include "budget_page.h"

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
      budgets(budgets),
      transactions(transactions),
      categories(categories),
      onChanged(std::move(onChanged)) {

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 28);
    root->setSpacing(16);

    QHBoxLayout* header = new QHBoxLayout();
    QLabel* title = new QLabel("Ngân sách");
    title->setObjectName("pageTitle");
    QPushButton* addButton = new QPushButton("+ Tạo ngân sách");
    addButton->setObjectName("primaryButton");
    header->addWidget(title);
    header->addStretch();
    header->addWidget(addButton);

    QFrame* card = new QFrame();
    card->setObjectName("card");
    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 18, 18, 18);

    QHBoxLayout* actions = new QHBoxLayout();
    QLabel* section = new QLabel("Danh sách ngân sách");
    section->setObjectName("sectionTitle");
    QPushButton* editButton = new QPushButton("Sửa");
    QPushButton* deleteButton = new QPushButton("Xóa");
    deleteButton->setObjectName("dangerButton");
    actions->addWidget(section);
    actions->addStretch();
    actions->addWidget(editButton);
    actions->addWidget(deleteButton);

    table = new QTableWidget(0, 7);
    table->setHorizontalHeaderLabels({
        "Danh mục", "Chu kỳ", "Khoảng thời gian",
        "Ngân sách", "Đã chi", "Còn lại", "Trạng thái"
    });
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->setVisible(false);

    cardLayout->addLayout(actions);
    cardLayout->addWidget(table);

    root->addLayout(header);
    root->addWidget(card);

    connect(addButton, &QPushButton::clicked, this, [this]() { addBudget(); });
    connect(editButton, &QPushButton::clicked, this, [this]() { editBudget(); });
    connect(deleteButton, &QPushButton::clicked, this, [this]() { deleteBudget(); });
    connect(table, &QTableWidget::cellDoubleClicked, this, [this](int, int) { editBudget(); });

    refresh();
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
