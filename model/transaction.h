#pragma once

#include <string>
using namespace std;

enum class TransactionType {
    INCOME,
    EXPENSE
};

string transactionTypeToString(TransactionType type);

class Transaction {
private:
    int id;
    double amount;
    string dateTime;
    string note;
    int walletId;
    int categoryId;

protected:
    Transaction();
    Transaction(int id, double amount, string dateTime, string note, int walletId, int categoryId);

public:
    virtual ~Transaction() = default;

    int getId() const;
    double getAmount() const;
    string getDateTime() const;
    string getNote() const;
    int getWalletId() const;
    int getCategoryId() const;

    virtual TransactionType getType() const = 0;
    virtual double getSignedAmount() const = 0;

    string getTypeString() const;
};
