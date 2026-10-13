#include "date_utils.h"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace {
    bool isLeapYear(int year) {
        return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
    }

    int daysInMonth(int year, int month) {
        static int days[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
        if (month == 2 && isLeapYear(year)) return 29;
        if (month < 1 || month > 12) return 0;
        return days[month];
    }

    bool parseDate(const string& date, int& year, int& month, int& day) {
        if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
        for (size_t i = 0; i < date.size(); ++i) {
            if (i != 4 && i != 7 && (date[i] < '0' || date[i] > '9')) return false;
        }
        try {
            year = stoi(date.substr(0,4));
            month = stoi(date.substr(5,2));
            day = stoi(date.substr(8,2));
        } catch (...) {
            return false;
        }
        if (year < 1900 || year > 9999 || month < 1 || month > 12) return false;
        if (day < 1 || day > daysInMonth(year, month)) return false;
        return true;
    }
}

string DateUtils::getCurrentDateTime() {
    time_t now = time(nullptr);
    tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    ostringstream out;
    out << put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return out.str();
}

string DateUtils::getCurrentDate() {
    return getCurrentDateTime().substr(0, 10);
}

bool DateUtils::isValidDate(const string& date) {
    int year, month, day;
    return parseDate(date, year, month, day);
}

bool DateUtils::isValidDateTime(const string& value) {
    if (!isValidDate(value.substr(0, 10))) return false;
    // Accept old files containing only a date as well as full timestamps.
    if (value.size() == 10) return true;
    if (value.size() != 19 || value[10] != ' ' || value[13] != ':' || value[16] != ':') return false;
    for (size_t i = 11; i < value.size(); ++i) {
        if (i != 13 && i != 16 && (value[i] < '0' || value[i] > '9')) return false;
    }
    return stoi(value.substr(11, 2)) < 24 && stoi(value.substr(14, 2)) < 60 &&
           stoi(value.substr(17, 2)) < 60;
}

string DateUtils::addDays(const string& date, int days) {
    int year, month, day;
    if (!parseDate(date, year, month, day)) return "";

    tm value{};
    value.tm_year = year - 1900;
    value.tm_mon = month - 1;
    value.tm_mday = day + days;
    value.tm_hour = 12;

    time_t t = mktime(&value);
    if (t == -1) return "";

    tm result{};
#ifdef _WIN32
    localtime_s(&result, &t);
#else
    localtime_r(&t, &result);
#endif
    ostringstream out;
    out << put_time(&result, "%Y-%m-%d");
    return out.str();
}

void DateUtils::getMonthRange(int year, int month, string& startDate, string& endDate) {
    ostringstream start, end;
    start << setw(4) << setfill('0') << year << "-"
          << setw(2) << month << "-01";
    end << setw(4) << setfill('0') << year << "-"
        << setw(2) << month << "-"
        << setw(2) << daysInMonth(year, month);
    startDate = start.str();
    endDate = end.str();
}

void DateUtils::getYearRange(int year, string& startDate, string& endDate) {
    startDate = to_string(year) + "-01-01";
    endDate = to_string(year) + "-12-31";
}

bool DateUtils::isDateInRange(const string& dateTime, const string& startDate, const string& endDate) {
    if (!isValidDateTime(dateTime)) return false;
    if (!startDate.empty() && !isValidDate(startDate)) return false;
    if (!endDate.empty() && !isValidDate(endDate)) return false;
    string date = dateTime.substr(0,10);
    return (startDate.empty() || date >= startDate) && (endDate.empty() || date <= endDate);
}
