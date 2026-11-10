#pragma once

#include <string>
#include "../model/user.h"
#include "../structure/linked_list.h"

// Quản lý danh sách tài khoản người dùng bằng LinkedList (UserManager)
class UserManager {
private:
    LinkedList<User> users;

public:
    UserManager();

    // Kiem tra ten dang nhap hop le: 1-20 ky tu, chi gom chu cai, chu so va dau gach duoi '_'
    static bool isValidUsername(const std::string& username);

    // Kiem tra mat khau hop le: toi thieu 4 ky tu, khong chua '|', '\n', '\r'
    static bool isValidPassword(const std::string& password);

    // Đọc / Ghi danh sách người dùng từ users.dat
    // Nếu file chưa tồn tại, bắt đầu với danh sách người dùng rỗng.
    bool loadUsers(const std::string& filePath);
    bool saveUsers(const std::string& filePath) const;

    // Đăng ký: tên không rỗng/không trùng, mật khẩu tối thiểu 4 ký tự
    bool registerUser(const std::string& username, const std::string& password, const std::string& name = "");

    // Đăng nhập: kiểm tra tài khoản và mật khẩu
    bool login(const std::string& username, const std::string& password) const;

    // Đổi mật khẩu: kiểm tra tài khoản, mật khẩu cũ đúng, mật khẩu mới >= 4 ký tự
    bool changePassword(const std::string& username, const std::string& oldPassword, const std::string& newPassword);

    // Tìm kiếm người dùng theo username
    User* findByUsername(const std::string& username);
    const User* findByUsername(const std::string& username) const;
    bool hasUser(const std::string& username) const;

    size_t getCount() const;
    void clear();

    LinkedList<User>& getUsers();
    const LinkedList<User>& getUsers() const;
};
