#include "category_manager.h"
#include <cctype>

// So sánh 2 chuỗi không phân biệt hoa thường
static bool equalsIgnoreCase(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}

// Chuẩn hóa khoảng trắng: cắt đầu đuôi và gộp các khoảng trắng liên tiếp
std::string CategoryManager::normalizeName(const std::string& str) {
    std::string res;
    size_t i = 0;
    while (i < str.size() && static_cast<unsigned char>(str[i]) <= ' ') ++i;
    bool space = false;
    for (; i < str.size(); ++i) {
        if (static_cast<unsigned char>(str[i]) <= ' ') {
            space = true;
        } else {
            if (space && !res.empty()) res += ' ';
            space = false;
            res += str[i];
        }
    }
    return res;
}

int CategoryManager::getNextId() const {
    int maxId = 0;
    for (Node<Category>* cur = categories.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data.getId() > maxId) {
            maxId = cur->data.getId();
        }
    }
    return maxId + 1;
}

bool CategoryManager::hasCategoryName(const std::string& name, CategoryType type, int excludeId) const {
    std::string norm = normalizeName(name);
    for (Node<Category>* cur = categories.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data.getId() != excludeId &&
            cur->data.getType() == type &&
            equalsIgnoreCase(normalizeName(cur->data.getName()), norm)) {
            return true;
        }
    }
    return false;
}

int CategoryManager::findCategoryId(const std::string& name, CategoryType type) const {
    std::string norm = normalizeName(name);
    if (norm.empty()) return 0;
    for (Node<Category>* cur = categories.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data.getType() == type &&
            equalsIgnoreCase(normalizeName(cur->data.getName()), norm)) {
            return cur->data.getId();
        }
    }
    return 0;
}

int CategoryManager::findOrCreateCategory(const std::string& name, CategoryType type) {
    std::string norm = normalizeName(name);
    if (norm.empty()) return 0;
    int existingId = findCategoryId(norm, type);
    if (existingId > 0) return existingId;

    int newId = 0;
    if (addCategory(norm, type, &newId)) return newId;
    return 0;
}

bool CategoryManager::addCategory(const std::string& name, CategoryType type, int* outId) {
    std::string norm = normalizeName(name);
    if (norm.empty()) return false;
    if (hasCategoryName(norm, type)) return false;

    int newId = getNextId();
    categories.pushBack(Category(newId, norm, type));
    if (outId) *outId = newId;
    return true;
}

bool CategoryManager::updateCategory(int id, const std::string& newName, CategoryType newType, bool isUsed) {
    if (newName.empty()) return false;
    Category* c = findById(id);
    if (!c) return false;
    if (c->getType() != newType && isUsed) return false;
    if (hasCategoryName(newName, newType, id)) return false;
    c->setName(newName);
    c->setType(newType);
    return true;
}

bool CategoryManager::updateCategoryName(int id, const std::string& newName) {
    Category* c = findById(id);
    if (!c) return false;
    return updateCategory(id, newName, c->getType(), false);
}

bool CategoryManager::deleteCategory(int id, bool isUsed) {
    // Nếu danh mục đang được sử dụng ở giao dịch thì từ chối xóa
    if (isUsed) return false;

    return categories.removeIf([id](const Category& c) {
        return c.getId() == id;
    });
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

size_t CategoryManager::getCount() const {
    return categories.getSize();
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
