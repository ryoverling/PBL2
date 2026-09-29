#pragma once

#include <string>
using namespace std;

class DateUtils {
public:
    static string getCurrentDateTime();
    static string getCurrentDate();
    static bool isValidDate(const string& date);
    static string addDays(const string& date, int days);
    static void getMonthRange(int year, int month, string& startDate, string& endDate);
    static void getYearRange(int year, string& startDate, string& endDate);
    static bool isDateInRange(const string& dateTime, const string& startDate, const string& endDate);
};
