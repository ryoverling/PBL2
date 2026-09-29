#include "budget.h"

Budget::Budget() {
    id = 0;
    amount = 0;
    startDate = "";
    endDate = "";
    categoryId = 0;
}

Budget::Budget(int id, double amount, string startDate, string endDate, int categoryId) {
    this->id = id;
    this->amount = amount;
    this->startDate = startDate;
    this->endDate = endDate;
    this->categoryId = categoryId;
}

int Budget::getId() const { return id; }
double Budget::getAmount() const { return amount; }
string Budget::getStartDate() const { return startDate; }
string Budget::getEndDate() const { return endDate; }
int Budget::getCategoryId() const { return categoryId; }

void Budget::setAmount(double amount) {
    if (amount > 0) this->amount = amount;
}

void Budget::setDateRange(string startDate, string endDate) {
    this->startDate = startDate;
    this->endDate = endDate;
}

void Budget::setCategoryId(int categoryId) {
    this->categoryId = categoryId;
}
