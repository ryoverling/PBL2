#include "file_storage.h"
#include "file_storage_internal.h"
#include <fstream>
#include <sstream>
#include <mutex>

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
