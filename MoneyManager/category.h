#pragma once

#include <string>
using namespace std;

enum class CategoryType {
    INCOME,
    EXPENSE
};

class Category {
private:
    int id;
    string name;
    CategoryType type;

public:
    Category();

    Category(int id, string name, CategoryType type);

    int getId() const;
    string getName() const;
    CategoryType getType() const;

    void setName(string name);

    string getTypeString() const;
};