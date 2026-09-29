#include "category.h"

string categoryTypeToString(CategoryType type) {
    return type == CategoryType::INCOME ? "INCOME" : "EXPENSE";
}

CategoryType categoryTypeFromString(const string& value) {
    if (value == "INCOME") {
        return CategoryType::INCOME;
    }
    return CategoryType::EXPENSE;
}

Category::Category() {
    id = 0;
    name = "";
    type = CategoryType::EXPENSE;
}

Category::Category(int id, string name, CategoryType type) {
    this->id = id;
    this->name = name;
    this->type = type;
}

int Category::getId() const {
    return id;
}

string Category::getName() const {
    return name;
}

CategoryType Category::getType() const {
    return type;
}

void Category::setName(string name) {
    this->name = name;
}

string Category::getTypeString() const {
    return type == CategoryType::INCOME ? "Thu nhap" : "Chi tieu";
}
