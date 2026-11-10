#include "format_utils.h"
#include <algorithm>
#include <cctype>
#include <cstdio>

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
