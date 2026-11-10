#pragma once

#include "transaction.h"

// Lớp đại diện cho Khoản Thu nhập
class Income : public Transaction {
public:
    Income();
    Income(int id, long long amount, const Date& date, const std::string& note, int walletId, int categoryId);

    // Ghi đè phương thức thuần ảo
    TransactionType getType() const override;
    // Thu nhập mang dấu dương (+)
    long long getSignedAmount() const override;
};
