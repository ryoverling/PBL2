#pragma once

#include "transaction.h"

class Income : public Transaction {
public:
    Income();
    Income(int id, double amount, string dateTime, string note, int walletId, int categoryId);

    TransactionType getType() const override;
    double getSignedAmount() const override;
};
