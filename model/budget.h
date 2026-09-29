#pragma once

#include <string>
using namespace std;

class Budget {
private:
    int id;
    double amount;
    string startDate;
    string endDate;
    int categoryId; // 0 = tat ca danh muc chi

public:
    Budget();
    Budget(int id, double amount, string startDate, string endDate, int categoryId);

    int getId() const;
    double getAmount() const;
    string getStartDate() const;
    string getEndDate() const;
    int getCategoryId() const;

    void setAmount(double amount);
    void setDateRange(string startDate, string endDate);
    void setCategoryId(int categoryId);
};
