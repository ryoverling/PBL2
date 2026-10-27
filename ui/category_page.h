#pragma once

#include <QWidget>
#include <functional>

class QComboBox;
class QTableWidget;

class CategoryManager;
class TransactionManager;
class BudgetManager;
namespace Ui { class CategoryPage; }

class CategoryPage : public QWidget {
private:
    Ui::CategoryPage* ui;
    CategoryManager* categories;
    TransactionManager* transactions;
    BudgetManager* budgets;
    std::function<void()> onChanged;

    QComboBox* filterCombo;
    QTableWidget* table;

    int selectedId() const;
    void addCategory();
    void editCategory();
    void deleteCategory();

public:
    CategoryPage(CategoryManager* categories,
                 TransactionManager* transactions,
                 BudgetManager* budgets,
                 std::function<void()> onChanged,
                 QWidget* parent = nullptr);
    ~CategoryPage();

    void refresh();
};
