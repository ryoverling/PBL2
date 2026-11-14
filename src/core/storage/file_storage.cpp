#include "file_storage.h"
#include "../model/transaction.h"
#include <fstream>
#include <filesystem>
#include <mutex>
#include <sstream>

namespace {

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

// Ghi tệp an toàn qua file tạm (.tmp) rồi đổi tên (rename) để giảm thiểu rủi ro tệp bị hỏng dở dang.
// Lưu ý: Trên Windows, rename sau remove chưa đảm bảo tính nguyên tử (atomic) tuyệt đối ở mức filesystem.
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

void clearOwnedTransactions(LinkedList<Transaction*>& transactions) {
    for (Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
        delete cur->data;
    }
    transactions.clear();
}

Transaction* parseTransaction(const RecordFields& fields) {
    int type = std::stoi(fields[0]);
    int id = std::stoi(fields[1]);
    long long amount = std::stoll(fields[2]);
    Date date = Date::fromString(fields[3]);
    std::string note = fields[4];
    int walletId = std::stoi(fields[5]);
    int categoryId = std::stoi(fields[6]);
    if (type == 1) return new Income(id, amount, date, note, walletId, categoryId);
    if (type == 2) return new Expense(id, amount, date, note, walletId, categoryId);
    return nullptr;
}

std::string safeTransactionNote(const std::string& note) {
    std::string safeNote = note;
    for (char& c : safeNote) if (c == '|') c = '/';
    return safeNote;
}

} // anonymous namespace

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

// 2. Đọc và Ghi danh sách Ví
bool FileStorage::loadWallets(const std::string& path, LinkedList<Wallet>& wallets) {
    std::lock_guard<std::mutex> lock(g_fileStorageMutex);
    std::ifstream file(path);
    if (!file.is_open()) return false;
    wallets.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        RecordFields fields = tachTruong(line);
        if (fields.size() < 4) continue;
        try {
            int id = std::stoi(fields[0]);
            std::string name = fields[1];
            WalletType type = walletTypeFromInt(std::stoi(fields[2]));
            long long balance = std::stoll(fields[3]);
            wallets.pushBack(Wallet(id, name, type, balance));
        } catch (...) { continue; }
    }
    return true;
}

bool FileStorage::saveWallets(const std::string& path, const LinkedList<Wallet>& wallets) {
    std::lock_guard<std::mutex> lock(g_fileStorageMutex);
    std::ostringstream ss;
    for (Node<Wallet>* cur = wallets.getHead(); cur != nullptr; cur = cur->next) {
        const Wallet& w = cur->data;
        ss << w.getId() << "|" << w.getName() << "|"
           << static_cast<int>(w.getType()) << "|" << w.getBalance() << "\n";
    }
    return commitAtomicFile(path, ss.str());
}

// 3. Đọc và Ghi danh sách Danh mục
bool FileStorage::loadCategories(const std::string& path, LinkedList<Category>& categories) {
    std::lock_guard<std::mutex> lock(g_fileStorageMutex);
    std::ifstream file(path);
    if (!file.is_open()) return false;
    categories.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        RecordFields fields = tachTruong(line);
        if (fields.size() < 3) continue;
        try {
            int id = std::stoi(fields[0]);
            std::string name = fields[1];
            CategoryType type = categoryTypeFromInt(std::stoi(fields[2]));
            categories.pushBack(Category(id, name, type));
        } catch (...) { continue; }
    }
    return true;
}

bool FileStorage::saveCategories(const std::string& path, const LinkedList<Category>& categories) {
    std::lock_guard<std::mutex> lock(g_fileStorageMutex);
    std::ostringstream ss;
    for (Node<Category>* cur = categories.getHead(); cur != nullptr; cur = cur->next) {
        const Category& c = cur->data;
        ss << c.getId() << "|" << c.getName() << "|"
           << static_cast<int>(c.getType()) << "\n";
    }
    return commitAtomicFile(path, ss.str());
}

// 4. Đọc và Ghi danh sách Giao dịch
bool FileStorage::loadTransactions(const std::string& path, LinkedList<Transaction*>& transactions) {
    std::lock_guard<std::mutex> lock(g_fileStorageMutex);
    std::ifstream file(path);
    if (!file.is_open()) return false;
    clearOwnedTransactions(transactions);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        RecordFields fields = tachTruong(line);
        if (fields.size() < 7) continue;
        try { if (Transaction* tx = parseTransaction(fields)) transactions.pushBack(tx); }
        catch (...) { continue; }
    }
    return true;
}

bool FileStorage::saveTransactions(const std::string& path, const LinkedList<Transaction*>& transactions) {
    std::lock_guard<std::mutex> lock(g_fileStorageMutex);
    std::ostringstream ss;
    for (Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
        Transaction* tx = cur->data;
        if (!tx) continue;
        int typeVal = (tx->getType() == TransactionType::INCOME) ? 1 : 2;
        // Thay the ky tu '|' trong ghi chu bang '/' de tranh hong dinh dang file
        std::string safeNote = safeTransactionNote(tx->getNote());
        ss << typeVal << "|" << tx->getId() << "|"
           << tx->getAmount() << "|"
           << tx->getDate().toFileString() << "|"
           << safeNote << "|"
           << tx->getWalletId() << "|"
           << tx->getCategoryId() << "\n";
    }
    return commitAtomicFile(path, ss.str());
}
