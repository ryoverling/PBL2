#include "transaction.h"

Transaction::Transaction()
    : id(0), amount(0), date(), note(""), walletId(0), categoryId(0) {}

Transaction::Transaction(int id, long long amount, const Date& date, const std::string& note, int walletId, int categoryId)
    : id(id), amount(amount), date(date), note(note), walletId(walletId), categoryId(categoryId) {}

int Transaction::getId() const { return id; }
long long Transaction::getAmount() const { return amount; }
Date Transaction::getDate() const { return date; }
std::string Transaction::getNote() const { return note; }
int Transaction::getWalletId() const { return walletId; }
int Transaction::getCategoryId() const { return categoryId; }

void Transaction::setAmount(long long newAmount) { amount = newAmount; }
void Transaction::setDate(const Date& newDate) { date = newDate; }
void Transaction::setNote(const std::string& newNote) { note = newNote; }
void Transaction::setWalletId(int newWalletId) { walletId = newWalletId; }
void Transaction::setCategoryId(int newCategoryId) { categoryId = newCategoryId; }

Income::Income() : Transaction() {}

Income::Income(int id, long long amount, const Date& date, const std::string& note, int walletId, int categoryId)
    : Transaction(id, amount, date, note, walletId, categoryId) {}

TransactionType Income::getType() const {
    return TransactionType::INCOME;
}

long long Income::getSignedAmount() const {
    return getAmount();
}

Expense::Expense() : Transaction() {}

Expense::Expense(int id, long long amount, const Date& date, const std::string& note, int walletId, int categoryId)
    : Transaction(id, amount, date, note, walletId, categoryId) {}

TransactionType Expense::getType() const {
    return TransactionType::EXPENSE;
}

long long Expense::getSignedAmount() const {
    return -getAmount();
}
