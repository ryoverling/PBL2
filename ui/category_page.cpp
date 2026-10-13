#include "category_page.h"

#include "category_dialog.h"
#include "ui_helpers.h"

#include "../manager/category_manager.h"
#include "../manager/transaction_manager.h"
#include "../manager/budget_manager.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

CategoryPage::CategoryPage(CategoryManager* categories,
                           TransactionManager* transactions,
                           BudgetManager* budgets,
                           std::function<void()> onChanged,
                           QWidget* parent)
    : QWidget(parent),
      categories(categories),
      transactions(transactions),
      budgets(budgets),
      onChanged(std::move(onChanged)) {

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 28);
    root->setSpacing(16);

    QHBoxLayout* header = new QHBoxLayout();
    QLabel* title = new QLabel("Danh mục");
    title->setObjectName("pageTitle");

    QPushButton* addButton = new QPushButton("+ Thêm danh mục");
    addButton->setObjectName("primaryButton");

    header->addWidget(title);
    header->addStretch();
    header->addWidget(addButton);

    QFrame* card = new QFrame();
    card->setObjectName("card");
    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 18, 18, 18);

    QHBoxLayout* tools = new QHBoxLayout();
    QLabel* filterLabel = new QLabel("Hiển thị");
    filterCombo = new QComboBox();
    filterCombo->addItem("Tất cả", -1);
    filterCombo->addItem("Chi tiêu", static_cast<int>(CategoryType::EXPENSE));
    filterCombo->addItem("Thu nhập", static_cast<int>(CategoryType::INCOME));

    QPushButton* editButton = new QPushButton("Sửa");
    QPushButton* deleteButton = new QPushButton("Xóa");
    deleteButton->setObjectName("dangerButton");

    tools->addWidget(filterLabel);
    tools->addWidget(filterCombo);
    tools->addStretch();
    tools->addWidget(editButton);
    tools->addWidget(deleteButton);

    table = new QTableWidget(0, 3);
    table->setHorizontalHeaderLabels({"ID", "Tên danh mục", "Loại"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->setVisible(false);

    cardLayout->addLayout(tools);
    cardLayout->addWidget(table);

    root->addLayout(header);
    root->addWidget(card);

    connect(addButton, &QPushButton::clicked, this, [this]() { addCategory(); });
    connect(editButton, &QPushButton::clicked, this, [this]() { editCategory(); });
    connect(deleteButton, &QPushButton::clicked, this, [this]() { deleteCategory(); });
    connect(filterCombo, &QComboBox::currentIndexChanged, this, [this]() { refresh(); });
    connect(table, &QTableWidget::cellDoubleClicked, this, [this](int, int) { editCategory(); });

    refresh();
}

int CategoryPage::selectedId() const {
    int row = table->currentRow();
    if (row < 0 || table->item(row, 0) == nullptr) return -1;
    return table->item(row, 0)->data(Qt::UserRole).toInt();
}

void CategoryPage::refresh() {
    table->setRowCount(0);
    int filter = filterCombo->currentData().toInt();

    categories->getCategories().forEach([&](const Category& category) {
        if (filter != -1 && static_cast<int>(category.getType()) != filter) return;

        int row = table->rowCount();
        table->insertRow(row);

        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(category.getId()));
        idItem->setData(Qt::UserRole, category.getId());

        table->setItem(row, 0, idItem);
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(category.getName())));
        table->setItem(row, 2, new QTableWidgetItem(UiHelpers::categoryTypeLabel(category.getType())));
    });
}

void CategoryPage::addCategory() {
    CategoryDialog dialog(nullptr, this);
    if (dialog.exec() != QDialog::Accepted) return;

    int id = categories->createCategory(
        dialog.categoryName().toStdString(),
        dialog.categoryType()
    );

    if (id < 0) {
        QMessageBox::warning(this, "Không thể thêm", "Dữ liệu danh mục không hợp lệ.");
        return;
    }

    if (onChanged) onChanged();
}

void CategoryPage::editCategory() {
    int id = selectedId();
    Category* category = categories->findById(id);
    if (category == nullptr) {
        QMessageBox::information(this, "Danh mục", "Vui lòng chọn một danh mục.");
        return;
    }

    CategoryDialog dialog(category, this);
    if (dialog.exec() != QDialog::Accepted) return;

    if (!categories->updateCategoryName(id, dialog.categoryName().toStdString())) {
        QMessageBox::warning(this, "Không thể sửa", "Tên danh mục không hợp lệ.");
        return;
    }
    if (onChanged) onChanged();
}

void CategoryPage::deleteCategory() {
    int id = selectedId();
    Category* category = categories->findById(id);
    if (category == nullptr) {
        QMessageBox::information(this, "Danh mục", "Vui lòng chọn một danh mục.");
        return;
    }

    if (QMessageBox::question(
            this,
            "Xóa danh mục",
            "Bạn có chắc muốn xóa danh mục \"" +
                QString::fromStdString(category->getName()) + "\"?"
        ) != QMessageBox::Yes) {
        return;
    }

    if (!categories->deleteCategory(id, *transactions, *budgets)) {
        QMessageBox::warning(this, "Không thể xóa", "Danh mục đang được sử dụng bởi giao dịch hoặc ngân sách.");
        return;
    }
    if (onChanged) onChanged();
}
