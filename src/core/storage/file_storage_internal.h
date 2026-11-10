#pragma once
#include <string>

// Mỗi dòng dữ liệu hiện có tối đa 7 trường (giao dịch).
struct RecordFields {
    static constexpr int MaxFields = 7;
    std::string values[MaxFields];
    int count = 0;

    int size() const { return count; }
    const std::string& operator[](int index) const { return values[index]; }
};

RecordFields tachTruong(const std::string& line, char delim = '|');

