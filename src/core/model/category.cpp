#include "category.h"

CategoryType categoryTypeFromInt(int typeVal) {
    if (typeVal == 1) return CategoryType::INCOME;
    return CategoryType::EXPENSE;
}

Category::Category() : id(0), name(""), type(CategoryType::EXPENSE) {}

Category::Category(int id, std::string name, CategoryType type)
    : id(id), name(name), type(type) {}

int Category::getId() const { return id; }
std::string Category::getName() const { return name; }
CategoryType Category::getType() const { return type; }

void Category::setName(const std::string& newName) { name = newName; }
void Category::setType(CategoryType newType) { type = newType; }
