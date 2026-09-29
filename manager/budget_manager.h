#pragma once

#include "../model/budget.h"
#include "../structure/linked_list.h"
#include "transaction_manager.h"

class BudgetManager {
private:
    LinkedList<Budget> budgets;

public:
    bool addBudget(const Budget& budget);
    int createBudget(double amount, string startDate, string endDate, int categoryId);
    bool deleteBudget(int id);
    bool updateBudgetAmount(int id, double amount);

    Budget* findById(int id);
    const Budget* findById(int id) const;

    double calculateSpent(const Budget& budget, const TransactionManager& transactionManager) const;
    double calculateRemaining(const Budget& budget, const TransactionManager& transactionManager) const;
    bool isOverBudget(const Budget& budget, const TransactionManager& transactionManager) const;

    bool hasCategoryBudget(int categoryId) const;
    int getNextId() const;
    void clear();

    LinkedList<Budget>& getBudgets();
    const LinkedList<Budget>& getBudgets() const;
};
