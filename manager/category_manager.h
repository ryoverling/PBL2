#pragma once

#include "../model/category.h"
#include "../structure/linked_list.h"

class TransactionManager;
class BudgetManager;

class CategoryManager {
private:
    LinkedList<Category> categories;

public:
    bool addCategory(const Category& category);
    int createCategory(string name, CategoryType type);
    bool deleteCategory(int id, const TransactionManager& transactions, const BudgetManager& budgets);
    bool updateCategoryName(int id, string newName);

    Category* findById(int id);
    const Category* findById(int id) const;

    int getNextId() const;
    void clear();

    void swap(CategoryManager& other);
    const LinkedList<Category>& getCategories() const;
};
