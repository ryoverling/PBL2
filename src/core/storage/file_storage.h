#pragma once

#include <string>
#include "../structure/linked_list.h"
#include "../model/user.h"
#include "../model/wallet.h"
#include "../model/category.h"
#include "../model/transaction.h"

// Cấu trúc phân tách các trường trong 1 dòng dữ liệu (tối đa 7 trường)
struct RecordFields {
    static constexpr int MaxFields = 7;
    std::string values[MaxFields];
    int count = 0;

    int size() const { return count; }
    const std::string& operator[](int index) const { return values[index]; }
};

RecordFields tachTruong(const std::string& line, char delim = '|');

// Lớp đọc và ghi 4 tệp dữ liệu .dat bằng ifstream / ofstream cơ bản
class FileStorage {
public:
    // Đọc / Ghi thông tin tài khoản người dùng
    static bool loadUser(const std::string& path, User& user);
    static bool saveUser(const std::string& path, const User& user);

    // Đọc / Ghi danh sách toàn bộ tài khoản người dùng (users.dat)
    static bool loadUsers(const std::string& path, LinkedList<User>& users);
    static bool saveUsers(const std::string& path, const LinkedList<User>& users);

    // Đọc / Ghi danh sách Ví tiền
    static bool loadWallets(const std::string& path, LinkedList<Wallet>& wallets);
    static bool saveWallets(const std::string& path, const LinkedList<Wallet>& wallets);

    // Đọc / Ghi danh sách Danh mục
    static bool loadCategories(const std::string& path, LinkedList<Category>& categories);
    static bool saveCategories(const std::string& path, const LinkedList<Category>& categories);

    // Đọc / Ghi danh sách Giao dịch (phân loại Income/Expense bằng if/else)
    static bool loadTransactions(const std::string& path, LinkedList<Transaction*>& transactions);
    static bool saveTransactions(const std::string& path, const LinkedList<Transaction*>& transactions);
};
