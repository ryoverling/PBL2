#include "category_manager.h"

bool CategoryManager::addCategory(const Category& category) {
    if (findById(category.getId()) != nullptr) return false;
    categories.pushBack(category);
    return true;
}

int CategoryManager::createCategory(string name, CategoryType type) {
    int id = getNextId();
    categories.pushBack(Category(id, name, type));
    return id;
}

bool CategoryManager::deleteCategory(int id) {
    return categories.removeIf([id](const Category& c) {
        return c.getId() == id;
    });
}

bool CategoryManager::updateCategoryName(int id, string newName) {
    Category* category = findById(id);
    if (category == nullptr || newName.empty()) return false;
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
    return maxId + 1;
}

void CategoryManager::clear() {
    categories.clear();
}

LinkedList<Category>& CategoryManager::getCategories() {
    return categories;
}

const LinkedList<Category>& CategoryManager::getCategories() const {
    return categories;
}
