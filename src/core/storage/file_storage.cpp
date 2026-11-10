#include "file_storage.h"
#include "file_storage_internal.h"
#include <fstream>
#include <filesystem>
#include <mutex>
#include <sstream>

std::mutex g_fileStorageMutex;

// Tạo thư mục cha nếu chưa tồn tại
void ensureParentDir(const std::string& filePath) {
    try {
        std::filesystem::path p(filePath);
        if (p.has_parent_path()) {
            std::filesystem::create_directories(p.parent_path());
        }
    } catch (...) {}
}

// Ghi file nguyên tử (atomic): ghi vào file .tmp rồi đổi tên (rename/replace)
bool commitAtomicFile(const std::string& targetPath, const std::string& content) {
    ensureParentDir(targetPath);
    std::string tmpPath = targetPath + ".tmp";
    {
        std::ofstream file(tmpPath, std::ios::binary);
        if (!file.is_open()) return false;
        file.write(content.data(), content.size());
        file.flush();
        if (!file.good()) return false;
    }
    std::error_code ec;
    std::filesystem::rename(tmpPath, targetPath, ec);
    if (ec) {
        std::filesystem::remove(targetPath, ec);
        std::filesystem::rename(tmpPath, targetPath, ec);
    }
    return !ec;
}

// Tách đúng số trường mà định dạng file sử dụng, giữ cả trường rỗng.
RecordFields tachTruong(const std::string& line, char delim) {
    RecordFields result;
    size_t start = 0;
    while (result.count < RecordFields::MaxFields) {
        size_t end = line.find(delim, start);
        result.values[result.count++] = line.substr(start, end == std::string::npos ? end : end - start);
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return result;
}

// 1. Đọc và Ghi thông tin User
bool FileStorage::loadUser(const std::string& path, User& user) {
    std::lock_guard<std::mutex> lock(g_fileStorageMutex);
    std::ifstream file(path);
    if (!file.is_open()) return false;
    std::string line;
    if (std::getline(file, line)) {
        RecordFields fields = tachTruong(line);
        if (fields.size() >= 3) {
            user = User(fields[0], fields[1], fields[2]);
            return true;
        }
    }
    return false;
}

bool FileStorage::saveUser(const std::string& path, const User& user) {
    std::lock_guard<std::mutex> lock(g_fileStorageMutex);
    std::ostringstream ss;
    ss << user.getUsername() << "|" << user.getPassword() << "|" << user.getName() << "\n";
    return commitAtomicFile(path, ss.str());
}

bool FileStorage::loadUsers(const std::string& path, LinkedList<User>& users) {
    std::lock_guard<std::mutex> lock(g_fileStorageMutex);
    std::ifstream file(path);
    if (!file.is_open()) return false;
    users.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        RecordFields fields = tachTruong(line);
        if (fields.size() >= 3) {
            users.pushBack(User(fields[0], fields[1], fields[2]));
        }
    }
    return true;
}

bool FileStorage::saveUsers(const std::string& path, const LinkedList<User>& users) {
    std::lock_guard<std::mutex> lock(g_fileStorageMutex);
    std::ostringstream ss;
    for (const Node<User>* cur = users.getHead(); cur != nullptr; cur = cur->next) {
        ss << cur->data.getUsername() << "|" << cur->data.getPassword() << "|" << cur->data.getName() << "\n";
    }
    return commitAtomicFile(path, ss.str());
}
