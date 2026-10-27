#pragma once

#include <QWidget>
#include <functional>

class QTableWidget;

class BudgetManager;
class TransactionManager;
class CategoryManager;
namespace Ui { class BudgetPage; }

class BudgetPage : public QWidget {
private:
    Ui::BudgetPage* ui;
    BudgetManager* budgets;
    TransactionManager* transactions;
    CategoryManager* categories;
    std::function<void()> onChanged;

    QTableWidget* table;

    int selectedId() const;
    QString periodLabel(const std::string& start, const std::string& end) const;

    void addBudget();
    void editBudget();
    void deleteBudget();

public:
    BudgetPage(BudgetManager* budgets,
               TransactionManager* transactions,
               CategoryManager* categories,
               std::function<void()> onChanged,
               QWidget* parent = nullptr);
    ~BudgetPage();

    void refresh();
};
