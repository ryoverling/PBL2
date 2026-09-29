#pragma once

#include "transaction.h"

class Expense : public Transaction {
public:
    Expense();
    Expense(int id, double amount, string dateTime, string note, int walletId, int categoryId);

    TransactionType getType() const override;
    double getSignedAmount() const override;
};
