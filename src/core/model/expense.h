#pragma once

#include "transaction.h"

// Lớp đại diện cho Khoản Chi tiêu
class Expense : public Transaction {
public:
    Expense();
    Expense(int id, long long amount, const Date& date, const std::string& note, int walletId, int categoryId);

    // Ghi đè phương thức thuần ảo
    TransactionType getType() const override;
    // Chi tiêu mang dấu âm (-)
    long long getSignedAmount() const override;
};
