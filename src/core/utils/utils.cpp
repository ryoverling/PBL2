#include "utils.h"
#include "../model/transaction.h"
#include "../model/category.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cmath>

// ============================================================================
// Implementation: Date
// ============================================================================
static bool isLeapYear(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

int Date::daysInMonth(int m, int y) {
    if (m < 1 || m > 12) return 0;
    const int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && isLeapYear(y)) return 29;
    return days[m];
}

int Date::weekdayOfFirstDay(int m, int y) {
    static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    int tempY = (m < 3) ? y - 1 : y;
    int dow = (tempY + tempY / 4 - tempY / 100 + tempY / 400 + t[m - 1] + 1) % 7;
    return (dow + 6) % 7;
}

Date::Date() : day(1), month(1), year(2026) {}
Date::Date(int d, int m, int y) : day(d), month(m), year(y) {}

bool Date::isValid() const {
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > daysInMonth(month, year)) return false;
    return true;
}

std::string Date::toString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << day << "/"
        << std::setfill('0') << std::setw(2) << month << "/" << year;
    return oss.str();
}

std::string Date::toFileString() const {
    std::ostringstream oss;
    oss << year << "-"
        << std::setfill('0') << std::setw(2) << month << "-"
        << std::setfill('0') << std::setw(2) << day;
    return oss.str();
}

Date Date::fromString(const std::string& str) {
    if (str.length() < 8) return Date();
    int d = 1, m = 1, y = 2026;
    char sep1, sep2;
    std::istringstream iss(str);
    if (str[4] == '-' || str[4] == '/') {
        iss >> y >> sep1 >> m >> sep2 >> d;
    } else {
        iss >> d >> sep1 >> m >> sep2 >> y;
    }
    return Date(d, m, y);
}

Date Date::today() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    if (!now) return Date(1, 1, 2026);
    return Date(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
}

bool Date::operator<(const Date& other) const {
    if (year != other.year) return year < other.year;
    if (month != other.month) return month < other.month;
    return day < other.day;
}

bool Date::operator>(const Date& other) const { return other < *this; }
bool Date::operator<=(const Date& other) const { return !(*this > other); }
bool Date::operator>=(const Date& other) const { return !(*this < other); }
bool Date::operator==(const Date& other) const {
    return day == other.day && month == other.month && year == other.year;
}
bool Date::operator!=(const Date& other) const { return !(*this == other); }

// ============================================================================
// Implementation: Tiện ích định dạng tiền tệ
// ============================================================================
std::string formatMoney(long long amount) {
    if (amount == 0) return "0";
    bool isNegative = (amount < 0);
    unsigned long long val = isNegative ? (0ULL - static_cast<unsigned long long>(amount)) 
                                        : static_cast<unsigned long long>(amount);
    std::string result = "";
    int count = 0;
    while (val > 0) {
        if (count > 0 && count % 3 == 0) result += '.';
        result += static_cast<char>('0' + (val % 10));
        val /= 10;
        count++;
    }
    if (isNegative) result += '-';
    std::reverse(result.begin(), result.end());
    return result;
}

long long parseMoney(const std::string& str) {
    long long sign = 1, val = 0;
    size_t i = 0;
    while (i < str.size() && std::isspace(static_cast<unsigned char>(str[i]))) i++;
    if (i < str.size() && str[i] == '-') { sign = -1; i++; }
    else if (i < str.size() && str[i] == '+') { i++; }
    for (; i < str.size(); ++i) {
        char c = str[i];
        if (c >= '0' && c <= '9') val = val * 10 + (c - '0');
        else if (c == '.' || c == ',' || c == ' ') continue;
        else break;
    }
    return sign * val;
}

std::string formatShortMoney(long long amount) {
    if (amount == 0) return "0";
    bool neg = amount < 0;
    long long val = neg ? -amount : amount;
    char buf[32];
    if (val >= 1000000) {
        snprintf(buf, sizeof(buf), "%.1fM", val / 1000000.0);
    } else if (val >= 1000) {
        if (val % 1000 == 0) snprintf(buf, sizeof(buf), "%lldk", val / 1000);
        else snprintf(buf, sizeof(buf), "%.1fk", val / 1000.0);
    } else {
        snprintf(buf, sizeof(buf), "%lld", val);
    }
    return (neg ? "-" : "") + std::string(buf);
}

// ============================================================================
// Implementation: Tiện ích thống kê (StatUtils)
// ============================================================================
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
    if (result.diffAmount > 0) {
        result.message = "Tăng";
    } else if (result.diffAmount < 0) {
        result.message = "Giảm";
    } else {
        result.message = "Không đổi";
    }
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
