#pragma once

#include <string>
#include "../model/category.h"
#include "../structure/linked_list.h"

// Quản lý danh mục Thu / Chi (phục vụ tự động tạo/tìm danh mục khi ghi nhận giao dịch)
class CategoryManager {
private:
    LinkedList<Category> categories;

    // Lấy ID tiếp theo = max(id) + 1, danh sách rỗng thì ID = 1
    int getNextId() const;

    // Kiểm tra tên danh mục đã tồn tại trong cùng nhóm loại (Thu/Chi) hay chưa
    bool hasCategoryName(const std::string& name, CategoryType type, int excludeId = 0) const;

    // Thêm danh mục mới nội bộ (phục vụ findOrCreateCategory khi lưu giao dịch)
    bool addCategory(const std::string& name, CategoryType type, int* outId = nullptr);

public:
    CategoryManager() = default;

    // Chuẩn hóa tên: cắt khoảng trắng đầu/cuối, gộp khoảng trắng thừa
    static std::string normalizeName(const std::string& str);

    // Tìm hoặc tạo mới danh mục: chuẩn hóa khoảng trắng, không phân biệt hoa thường
    // Trả về ID danh mục hiện có nếu trùng tên & cùng loại, ngược lại tạo mới và trả về ID mới
    int findOrCreateCategory(const std::string& name, CategoryType type);

    // Tìm ID danh mục theo tên và loại (trả về 0 nếu không tìm thấy)
    int findCategoryId(const std::string& name, CategoryType type) const;

    // Tìm danh mục theo ID
    Category* findById(int id);
    const Category* findById(int id) const;

    // Lấy số lượng danh mục
    size_t getCount() const;

    // Xóa toàn bộ danh sách danh mục
    void clear();

    // Truy xuất danh sách danh mục
    LinkedList<Category>& getCategories();
    const LinkedList<Category>& getCategories() const;
};
