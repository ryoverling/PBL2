#include "stat_utils.h"
#include <cmath>

namespace StatUtils {

TransactionTotals sumByType(const LinkedList<Transaction*>& transactions) {
    TransactionTotals totals;
    for (const Node<Transaction*>* cur = transactions.getHead(); cur; cur = cur->next) {
        const Transaction* tx = cur->data;
        if (!tx) continue;
        ++totals.count;
        if (tx->getType() == TransactionType::INCOME) totals.income += tx->getAmount();
        else totals.expense += tx->getAmount();
    }
    return totals;
}

bool cungThangNam(const Transaction* giaoDich, int thang, int nam) {
    return giaoDich && giaoDich->getDate().month == thang && giaoDich->getDate().year == nam;
}

void getMonthlyTotals(int month, int year, const LinkedList<Transaction*>& transactions,
                      long long& outIncome, long long& outExpense) {
    outIncome = 0;
    outExpense = 0;
    for (const Node<Transaction*>* cur = transactions.getHead(); cur; cur = cur->next) {
        const Transaction* tx = cur->data;
        if (!cungThangNam(tx, month, year)) continue;
        if (tx->getType() == TransactionType::INCOME) outIncome += tx->getAmount();
        else outExpense += tx->getAmount();
    }
}

DailyExpenseList getExpenseByDay(const LinkedList<Transaction*>& all, int month, int year) {
    DailyExpenseList res;
    if (!Date(1, month, year).isValid()) return res;
    int lastDay = Date::daysInMonth(month, year);
    for (const Node<Transaction*>* cur = all.getHead(); cur; cur = cur->next) {
        const Transaction* tx = cur->data;
        if (!cungThangNam(tx, month, year) || tx->getType() != TransactionType::EXPENSE) continue;
        int day = tx->getDate().day;
        if (day >= 1 && day <= lastDay) res[day - 1] += tx->getAmount();
    }
    return res;
}

void get6MonthsStats(int endMonth, int endYear, const LinkedList<Transaction*>& transactions,
                     MonthStat outStats[6]) {
    for (int i = 0; i < 6; ++i) {
        int month = endMonth - (5 - i);
        int year = endYear;
        while (month <= 0) {
            month += 12;
            year -= 1;
        }
        outStats[i].month = month;
        outStats[i].year = year;
        getMonthlyTotals(month, year, transactions, outStats[i].totalIncome, outStats[i].totalExpense);
    }
}

ExpenseComparison compareExpenseWithPreviousMonth(int month, int year,
                                                  const LinkedList<Transaction*>& transactions) {
    int previousMonth = month == 1 ? 12 : month - 1;
    int previousYear = month == 1 ? year - 1 : year;
    long long income = 0, expense = 0, previousExpense = 0;
    getMonthlyTotals(month, year, transactions, income, expense);
    getMonthlyTotals(previousMonth, previousYear, transactions, income, previousExpense);
    ExpenseComparison result;
    result.diffAmount = expense - previousExpense;
    if (previousExpense == 0) return result;
    result.canCompare = true;
    result.diffPercent = result.diffAmount * 100.0 / previousExpense;
    result.message = result.diffAmount > 0 ? "Tăng" : (result.diffAmount < 0 ? "Giảm" : "Không đổi");
    return result;
}

static std::string tenDanhMuc(int id, const LinkedList<Category>& categories) {
    for (const Node<Category>* cur = categories.getHead(); cur; cur = cur->next) {
        if (cur->data.getId() == id) return cur->data.getName();
    }
    return "Khác";
}

static void congChiDanhMuc(LinkedList<CategoryExpense>& list, const std::string& name, long long amount) {
    for (Node<CategoryExpense>* cur = list.getHead(); cur; cur = cur->next) {
        if (cur->data.categoryName == name) {
            cur->data.totalExpense += amount;
            return;
        }
    }
    CategoryExpense item;
    item.categoryName = name;
    item.totalExpense = amount;
    list.pushBack(item);
}

static bool chiGiamDan(const CategoryExpense& a, const CategoryExpense& b) {
    return a.totalExpense > b.totalExpense;
}

static void tinhTyTrong(long long totalExpense, LinkedList<CategoryExpense>& list) {
    // Làm tròn độc lập, không dồn phần dư; tổng tỷ trọng có thể khác 100%.
    for (Node<CategoryExpense>* cur = list.getHead(); cur; cur = cur->next) {
        cur->data.percentage = 0;
        if (totalExpense > 0) {
            cur->data.percentage = static_cast<int>(std::round(cur->data.totalExpense * 100.0 / totalExpense));
        }
    }
}

void getExpenseByCategory(int month, int year,
                          const LinkedList<Transaction*>& transactions,
                          const LinkedList<Category>& categories,
                          LinkedList<CategoryExpense>& outList) {
    outList.clear();
    long long income = 0, expense = 0;
    getMonthlyTotals(month, year, transactions, income, expense);
    for (const Node<Transaction*>* cur = transactions.getHead(); cur; cur = cur->next) {
        const Transaction* tx = cur->data;
        if (!cungThangNam(tx, month, year) || tx->getType() != TransactionType::EXPENSE) continue;
        congChiDanhMuc(outList, tenDanhMuc(tx->getCategoryId(), categories), tx->getAmount());
    }
    outList.selectionSort(chiGiamDan);
    tinhTyTrong(expense, outList);
}

} // namespace StatUtils
