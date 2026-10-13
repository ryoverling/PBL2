#include "budget_manager.h"
#include "../utils/date_utils.h"
#include <cmath>
#include <climits>

namespace {
    bool validBudget(double amount, const string& start, const string& end, int categoryId) {
        return std::isfinite(amount) && amount > 0 && DateUtils::isValidDate(start) &&
               DateUtils::isValidDate(end) && start <= end && categoryId >= 0;
    }
    bool validCategory(int id, const CategoryManager& categories) {
        if (id == 0) return true;
        const Category* category = categories.findById(id);
        return category != nullptr && category->getType() == CategoryType::EXPENSE;
    }
}

bool BudgetManager::addBudget(const Budget& budget) {
    if (budget.getId() <= 0 || findById(budget.getId()) != nullptr ||
        !validBudget(budget.getAmount(), budget.getStartDate(), budget.getEndDate(), budget.getCategoryId())) return false;
    budgets.pushBack(budget);
    return true;
}

int BudgetManager::createBudget(double amount, string startDate, string endDate, int categoryId,
                               const CategoryManager& categories) {
    if (!validCategory(categoryId, categories)) return -1;
    int id = getNextId();
    return addBudget(Budget(id, amount, startDate, endDate, categoryId)) ? id : -1;
}

bool BudgetManager::updateBudget(int id, double amount, string startDate, string endDate, int categoryId,
                               const CategoryManager& categories) {
    Budget* budget = findById(id);
    if (budget == nullptr || !validBudget(amount, startDate, endDate, categoryId) ||
        !validCategory(categoryId, categories)) return false;
    budget->setAmount(amount);
    budget->setDateRange(startDate, endDate);
    budget->setCategoryId(categoryId);
    return true;
}

bool BudgetManager::hasValidReferences(const CategoryManager& categories) const {
    bool valid = true;
    budgets.forEach([&](const Budget& budget) {
        if (!validCategory(budget.getCategoryId(), categories)) valid = false;
    });
    return valid;
}

bool BudgetManager::deleteBudget(int id) {
    return budgets.removeIf([id](const Budget& b) {
        return b.getId() == id;
    });
}

bool BudgetManager::updateBudgetAmount(int id, double amount) {
    Budget* budget = findById(id);
    if (budget == nullptr || !std::isfinite(amount) || amount <= 0) return false;
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
    return maxId == INT_MAX ? -1 : maxId + 1;
}

void BudgetManager::clear() {
    budgets.clear();
}

void BudgetManager::swap(BudgetManager& other) {
    budgets.swap(other.budgets);
}

const LinkedList<Budget>& BudgetManager::getBudgets() const {
    return budgets;
}
