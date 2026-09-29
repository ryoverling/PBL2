#pragma once

#include <QWidget>
#include <functional>

class QComboBox;
class QTableWidget;

class CategoryManager;
class TransactionManager;
class BudgetManager;

class CategoryPage : public QWidget {
private:
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

    void refresh();
};
