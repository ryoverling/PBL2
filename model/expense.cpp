#include "expense.h"

Expense::Expense() : Transaction() {}

Expense::Expense(int id, double amount, string dateTime, string note, int walletId, int categoryId)
    : Transaction(id, amount, dateTime, note, walletId, categoryId) {}

TransactionType Expense::getType() const {
    return TransactionType::EXPENSE;
}

double Expense::getSignedAmount() const {
    return -getAmount();
}
