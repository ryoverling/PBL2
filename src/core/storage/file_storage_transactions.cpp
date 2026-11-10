#include "file_storage.h"
#include "file_storage_internal.h"
#include "../model/income.h"
#include "../model/expense.h"
#include <fstream>
#include <sstream>
#include <mutex>

// 4. Đọc và Ghi danh sách Giao dịch
static void clearOwnedTransactions(LinkedList<Transaction*>& transactions) {
    for (Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
        delete cur->data;
    }
    transactions.clear();
}

static Transaction* parseTransaction(const RecordFields& fields) {
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

static std::string safeTransactionNote(const std::string& note) {
    std::string safeNote = note;
    for (char& c : safeNote) if (c == '|') c = '/';
    return safeNote;
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
