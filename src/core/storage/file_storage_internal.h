#pragma once
#include <string>
#include <mutex>

extern std::mutex g_fileStorageMutex;

void ensureParentDir(const std::string& filePath);
bool commitAtomicFile(const std::string& targetPath, const std::string& content);

// Mỗi dòng dữ liệu hiện có tối đa 7 trường (giao dịch).
struct RecordFields {
    static constexpr int MaxFields = 7;
    std::string values[MaxFields];
    int count = 0;

    int size() const { return count; }
    const std::string& operator[](int index) const { return values[index]; }
};

RecordFields tachTruong(const std::string& line, char delim = '|');
