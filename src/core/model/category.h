#pragma once

#include <string>

// Phân loại danh mục
enum class CategoryType {
    INCOME = 1,  // Danh mục Thu nhập
    EXPENSE = 2  // Danh mục Chi tiêu
};

CategoryType categoryTypeFromInt(int typeVal);

// Thực thể Danh mục
class Category {
private:
    int id;
    std::string name;
    CategoryType type;

public:
    Category();
    Category(int id, std::string name, CategoryType type);

    int getId() const;
    std::string getName() const;
    CategoryType getType() const;

    void setName(const std::string& name);
    void setType(CategoryType type);

};
