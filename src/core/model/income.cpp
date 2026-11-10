#include "income.h"

Income::Income() : Transaction() {}

Income::Income(int id, long long amount, const Date& date, const std::string& note, int walletId, int categoryId)
    : Transaction(id, amount, date, note, walletId, categoryId) {}

TransactionType Income::getType() const {
    return TransactionType::INCOME;
}

long long Income::getSignedAmount() const {
    return getAmount();
}
