#pragma once

#include <string>
#include "../model/wallet.h"
#include "../structure/linked_list.h"

// Quản lý danh sách Ví tiền (độc lập, không phụ thuộc Manager khác)
class WalletManager {
private:
    LinkedList<Wallet> wallets;

public:
    WalletManager() = default;

    // Lấy ID tiếp theo = max(id) + 1, danh sách rỗng thì ID = 1
    int getNextId() const;

    // Kiểm tra tên ví đã tồn tại hay chưa
    bool hasWalletName(const std::string& name, int excludeId = 0) const;

    // Thêm ví mới: tên không rỗng, không trùng tên, số dư ban đầu >= 0
    bool addWallet(const std::string& name, WalletType type, long long initialBalance, int* outId = nullptr);

    // Sửa tên và loại ví
    bool updateWallet(int id, const std::string& newName, WalletType newType);

    // Xóa ví: nếu isUsed == true thì từ chối xóa và trả về false
    bool deleteWallet(int id, bool isUsed);

    // Tìm ví theo ID
    Wallet* findById(int id);
    const Wallet* findById(int id) const;

    // Tính tổng số dư tất cả các ví
    long long getTotalBalance() const;

    // Lấy số lượng ví
    size_t getCount() const;

    // Xóa toàn bộ danh sách ví
    void clear();

    // Truy xuất danh sách ví
    LinkedList<Wallet>& getWallets();
    const LinkedList<Wallet>& getWallets() const;
};
