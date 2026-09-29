#include "income.h"

Income::Income() : Transaction() {}

Income::Income(int id, double amount, string dateTime, string note, int walletId, int categoryId)
    : Transaction(id, amount, dateTime, note, walletId, categoryId) {}

TransactionType Income::getType() const {
    return TransactionType::INCOME;
}

double Income::getSignedAmount() const {
    return getAmount();
}
