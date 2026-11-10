#include "wallet_manager.h"
#include <cctype>

// So sánh 2 chuỗi không phân biệt hoa thường
static bool equalsIgnoreCase(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}

int WalletManager::getNextId() const {
    int maxId = 0;
    for (Node<Wallet>* cur = wallets.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data.getId() > maxId) {
            maxId = cur->data.getId();
        }
    }
    return maxId + 1;
}

bool WalletManager::hasWalletName(const std::string& name, int excludeId) const {
    for (Node<Wallet>* cur = wallets.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data.getId() != excludeId && equalsIgnoreCase(cur->data.getName(), name)) {
            return true;
        }
    }
    return false;
}

bool WalletManager::addWallet(const std::string& name, WalletType type, long long initialBalance, int* outId) {
    // Kiểm tra dữ liệu hợp lệ tối thiểu
    if (name.empty()) return false;
    if (initialBalance < 0) return false;
    if (hasWalletName(name)) return false;

    int newId = getNextId();
    wallets.pushBack(Wallet(newId, name, type, initialBalance));
    if (outId) *outId = newId;
    return true;
}

bool WalletManager::updateWallet(int id, const std::string& newName, WalletType newType) {
    if (newName.empty()) return false;
    if (hasWalletName(newName, id)) return false;

    Wallet* w = findById(id);
    if (!w) return false;

    w->setName(newName);
    w->setType(newType);
    return true;
}

bool WalletManager::deleteWallet(int id, bool isUsed) {
    // Nếu ví đang có giao dịch sử dụng thì từ chối xóa
    if (isUsed) return false;

    return wallets.removeIf([id](const Wallet& w) {
        return w.getId() == id;
    });
}

Wallet* WalletManager::findById(int id) {
    return wallets.findIf([id](const Wallet& w) {
        return w.getId() == id;
    });
}

const Wallet* WalletManager::findById(int id) const {
    return wallets.findIf([id](const Wallet& w) {
        return w.getId() == id;
    });
}

long long WalletManager::getTotalBalance() const {
    long long total = 0;
    for (Node<Wallet>* cur = wallets.getHead(); cur != nullptr; cur = cur->next) {
        total += cur->data.getBalance();
    }
    return total;
}

size_t WalletManager::getCount() const {
    return wallets.getSize();
}

void WalletManager::clear() {
    wallets.clear();
}

LinkedList<Wallet>& WalletManager::getWallets() {
    return wallets;
}

const LinkedList<Wallet>& WalletManager::getWallets() const {
    return wallets;
}
