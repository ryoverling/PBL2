#include "category.h"

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
    if (type == CategoryType::INCOME) {
        return "Thu nhap";
    }

    return "Chi tieu";
}