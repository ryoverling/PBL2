#pragma once

#include <string>
#include "../utils/date.h"

// Phân loại giao dịch
enum class TransactionType {
    INCOME = 1,  // Khoản thu
    EXPENSE = 2  // Khoản chi
};


// Lớp cơ sở trừu tượng đại diện cho một Giao dịch tài chính
class Transaction {
private:
    int id;
    long long amount;      // Số tiền (VNĐ, luôn dương)
    Date date;             // Ngày phát sinh giao dịch
    std::string note;      // Ghi chú chi tiết
    int walletId;          // Mã ví thực hiện giao dịch
    int categoryId;        // Mã danh mục tương ứng

protected:
    Transaction();
    Transaction(int id, long long amount, const Date& date, const std::string& note, int walletId, int categoryId);

public:
    // Destructor ảo: Bắt buộc để khi giải phóng qua con trỏ Transaction*,
    // destructor của lớp con (Income/Expense) được gọi đúng cách
    virtual ~Transaction() = default;

    int getId() const;
    long long getAmount() const;
    Date getDate() const;
    std::string getNote() const;
    int getWalletId() const;
    int getCategoryId() const;

    void setAmount(long long newAmount);
    void setDate(const Date& newDate);
    void setNote(const std::string& newNote);
    void setWalletId(int newWalletId);
    void setCategoryId(int newCategoryId);

    // Phương thức thuần ảo xác định loại giao dịch
    virtual TransactionType getType() const = 0;

    // Phương thức thuần ảo trả về số tiền có dấu (+amount cho Thu, -amount cho Chi)
    // để cộng trừ trực tiếp vào số dư ví
    virtual long long getSignedAmount() const = 0;

};
