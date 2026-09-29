#include "transaction.h"
#include "../utils/date_utils.h"

string transactionTypeToString(TransactionType type) {
    return type == TransactionType::INCOME ? "INCOME" : "EXPENSE";
}

Transaction::Transaction() {
    id = 0;
    amount = 0;
    dateTime = DateUtils::getCurrentDateTime();
    note = "";
    walletId = 0;
    categoryId = 0;
}

Transaction::Transaction(int id, double amount, string dateTime, string note, int walletId, int categoryId) {
    this->id = id;
    this->amount = amount;
    this->dateTime = dateTime.empty() ? DateUtils::getCurrentDateTime() : dateTime;
    this->note = note;
    this->walletId = walletId;
    this->categoryId = categoryId;
}

int Transaction::getId() const {
    return id;
}

double Transaction::getAmount() const {
    return amount;
}

string Transaction::getDateTime() const {
    return dateTime;
}

string Transaction::getNote() const {
    return note;
}

int Transaction::getWalletId() const {
    return walletId;
}

int Transaction::getCategoryId() const {
    return categoryId;
}

string Transaction::getTypeString() const {
    return getType() == TransactionType::INCOME ? "Thu nhap" : "Chi tieu";
}
