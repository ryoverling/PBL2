#include "category_manager.h"
#include "transaction_manager.h"
#include "budget_manager.h"
#include <climits>

bool CategoryManager::addCategory(const Category& category) {
    if (category.getId() <= 0 || category.getName().find_first_not_of(" \t\r\n") == string::npos ||
        (category.getType() != CategoryType::INCOME && category.getType() != CategoryType::EXPENSE) ||
        findById(category.getId()) != nullptr) return false;
    categories.pushBack(category);
    return true;
}

int CategoryManager::createCategory(string name, CategoryType type) {
    int id = getNextId();
    return addCategory(Category(id, name, type)) ? id : -1;
}

bool CategoryManager::deleteCategory(int id, const TransactionManager& transactions, const BudgetManager& budgets) {
    if (transactions.hasCategoryTransactions(id) || budgets.hasCategoryBudget(id)) return false;
    return categories.removeIf([id](const Category& c) {
        return c.getId() == id;
    });
}

bool CategoryManager::updateCategoryName(int id, string newName) {
    Category* category = findById(id);
    if (category == nullptr || newName.find_first_not_of(" \t\r\n") == string::npos) return false;
    category->setName(newName);
    return true;
}

Category* CategoryManager::findById(int id) {
    return categories.findIf([id](const Category& c) {
        return c.getId() == id;
    });
}

const Category* CategoryManager::findById(int id) const {
    return categories.findIf([id](const Category& c) {
        return c.getId() == id;
    });
}

int CategoryManager::getNextId() const {
    int maxId = 0;
    categories.forEach([&maxId](const Category& c) {
        if (c.getId() > maxId) maxId = c.getId();
    });
    return maxId == INT_MAX ? -1 : maxId + 1;
}

void CategoryManager::clear() {
    categories.clear();
}

void CategoryManager::swap(CategoryManager& other) {
    categories.swap(other.categories);
}

const LinkedList<Category>& CategoryManager::getCategories() const {
    return categories;
}
