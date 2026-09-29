#include "budget_manager.h"
#include "../utils/date_utils.h"

bool BudgetManager::addBudget(const Budget& budget) {
    if (findById(budget.getId()) != nullptr || budget.getAmount() <= 0) return false;
    budgets.pushBack(budget);
    return true;
}

int BudgetManager::createBudget(double amount, string startDate, string endDate, int categoryId) {
    if (amount <= 0 || startDate > endDate) return -1;
    int id = getNextId();
    budgets.pushBack(Budget(id, amount, startDate, endDate, categoryId));
    return id;
}

bool BudgetManager::deleteBudget(int id) {
    return budgets.removeIf([id](const Budget& b) {
        return b.getId() == id;
    });
}

bool BudgetManager::updateBudgetAmount(int id, double amount) {
    Budget* budget = findById(id);
    if (budget == nullptr || amount <= 0) return false;
    budget->setAmount(amount);
    return true;
}

Budget* BudgetManager::findById(int id) {
    return budgets.findIf([id](const Budget& b) {
        return b.getId() == id;
    });
}

const Budget* BudgetManager::findById(int id) const {
    return budgets.findIf([id](const Budget& b) {
        return b.getId() == id;
    });
}

double BudgetManager::calculateSpent(const Budget& budget, const TransactionManager& transactionManager) const {
    double spent = 0;
    transactionManager.getTransactions().forEach([&](Transaction* const& t) {
        if (t == nullptr || t->getType() != TransactionType::EXPENSE) return;
        if (!DateUtils::isDateInRange(t->getDateTime(), budget.getStartDate(), budget.getEndDate())) return;
        if (budget.getCategoryId() != 0 && t->getCategoryId() != budget.getCategoryId()) return;
        spent += t->getAmount();
    });
    return spent;
}

double BudgetManager::calculateRemaining(const Budget& budget, const TransactionManager& transactionManager) const {
    return budget.getAmount() - calculateSpent(budget, transactionManager);
}

bool BudgetManager::isOverBudget(const Budget& budget, const TransactionManager& transactionManager) const {
    return calculateSpent(budget, transactionManager) > budget.getAmount();
}

bool BudgetManager::hasCategoryBudget(int categoryId) const {
    bool found = false;
    budgets.forEach([&](const Budget& b) {
        if (b.getCategoryId() == categoryId) found = true;
    });
    return found;
}

int BudgetManager::getNextId() const {
    int maxId = 0;
    budgets.forEach([&](const Budget& b) {
        if (b.getId() > maxId) maxId = b.getId();
    });
    return maxId + 1;
}

void BudgetManager::clear() {
    budgets.clear();
}

LinkedList<Budget>& BudgetManager::getBudgets() {
    return budgets;
}

const LinkedList<Budget>& BudgetManager::getBudgets() const {
    return budgets;
}
