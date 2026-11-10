#include "expense.h"

Expense::Expense() : Transaction() {}

Expense::Expense(int id, long long amount, const Date& date, const std::string& note, int walletId, int categoryId)
    : Transaction(id, amount, date, note, walletId, categoryId) {}

TransactionType Expense::getType() const {
    return TransactionType::EXPENSE;
}

long long Expense::getSignedAmount() const {
    return -getAmount();
}
