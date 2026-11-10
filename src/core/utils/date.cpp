#include "date.h"
#include <ctime>
#include <sstream>
#include <iomanip>

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
