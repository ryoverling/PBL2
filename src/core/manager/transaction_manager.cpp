#include "transaction_manager.h"
#include "../utils/utils.h"
#include <vector>
#include <algorithm>
#include <cctype>

void TransactionManager::applyToWallet(const Transaction* tx, Wallet* wallet, bool isReverse) {
    if (!tx || !wallet) return;
    long long change = tx->getSignedAmount();
    if (isReverse) change = -change;
    if (change > 0) wallet->deposit(change);
    else if (change < 0) wallet->withdraw(-change);
}

TransactionManager::~TransactionManager() {
    clear();
}

int TransactionManager::getNextId() const {
    int maxId = 0;
    for (Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data && cur->data->getId() > maxId) {
            maxId = cur->data->getId();
        }
    }
    return maxId + 1;
}

long long TransactionManager::getWalletInitialBalance(int walletId, long long currentBalance) const {
    long long sumSigned = 0;
    for (const Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data && cur->data->getWalletId() == walletId) {
            sumSigned += cur->data->getSignedAmount();
        }
    }
    return currentBalance - sumSigned;
}

BalanceValidationResult TransactionManager::validateWalletTimeline(
    int walletId,
    long long initialBalance,
    const LinkedList<Transaction*>& simulatedTxList,
    const std::string& walletName) const {

    BalanceValidationResult res;
    res.walletName = walletName;

    if (initialBalance < 0) {
        res.isValid = false;
        res.shortfall = -initialBalance;
        res.errorMessage = "Số dư ban đầu của ví không hợp lệ (nhỏ hơn 0).";
        return res;
    }

    std::vector<Transaction*> sorted;
    for (const Node<Transaction*>* cur = simulatedTxList.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data && cur->data->getWalletId() == walletId) {
            sorted.push_back(cur->data);
        }
    }

    // Sắp xếp trình tự thời gian theo quy tắc ổn định:
    // 1. Ngày tăng dần
    // 2. Cùng ngày: Khoản Thu trước Khoản Chi (tiền vào trước khi tiền ra)
    // 3. Cùng ngày và cùng loại: theo ID tăng dần
    std::sort(sorted.begin(), sorted.end(), [](const Transaction* a, const Transaction* b) {
        if (!a || !b) return false;
        if (a->getDate() < b->getDate()) return true;
        if (b->getDate() < a->getDate()) return false;
        bool aInc = (a->getType() == TransactionType::INCOME);
        bool bInc = (b->getType() == TransactionType::INCOME);
        if (aInc != bInc) return aInc;
        return a->getId() < b->getId();
    });

    long long runningBalance = initialBalance;
    for (const Transaction* tx : sorted) {
        if (!tx) continue;
        long long balanceBefore = runningBalance;
        runningBalance += tx->getSignedAmount();
        if (runningBalance < 0) {
            res.isValid = false;
            res.errorCode = ValidationErrorCode::WOULD_GO_NEGATIVE_ON_DATE;
            res.violationDate = tx->getDate();
            res.shortfall = -runningBalance;
            res.requiredAmount = tx->getAmount();
            res.availableBalance = balanceBefore;
            res.walletName = walletName;
            res.errorMessage = "Thao tác này làm ví " + walletName +
                               " âm " + formatMoney(res.shortfall) + " đ vào ngày " +
                               tx->getDate().toString() + ".";
            return res;
        }
    }

    res.isValid = true;
    res.errorCode = ValidationErrorCode::NONE;
    res.availableBalance = runningBalance;
    return res;
}

BalanceValidationResult TransactionManager::validateAdd(const Transaction* tx, const Wallet* wallet) const {
    BalanceValidationResult res;
    if (!tx || !wallet) {
        res.isValid = false;
        res.errorMessage = "Dữ liệu giao dịch hoặc ví không hợp lệ.";
        return res;
    }
    res.walletName = wallet->getName();
    res.requiredAmount = tx->getAmount();
    res.availableBalance = wallet->getBalance();

    if (tx->getAmount() <= 0) {
        res.isValid = false;
        res.errorCode = ValidationErrorCode::INVALID_AMOUNT;
        res.errorMessage = "Số tiền phải lớn hơn 0.";
        return res;
    }
    if (!tx->getDate().isValid()) {
        res.isValid = false;
        res.errorMessage = "Ngày giao dịch không hợp lệ.";
        return res;
    }

    if (tx->getType() == TransactionType::INCOME) {
        // Cho phép thêm thu nhập vào bất kỳ ví nào (kể cả ví đang âm để bù âm)
        res.isValid = true;
        res.errorCode = ValidationErrorCode::NONE;
        return res;
    }

    // Nếu ví đang âm mà cố chi thêm: chặn ngay với WALLET_ALREADY_NEGATIVE
    if (wallet->getBalance() < 0) {
        res.isValid = false;
        res.errorCode = ValidationErrorCode::WALLET_ALREADY_NEGATIVE;
        res.shortfall = -wallet->getBalance() + tx->getAmount();
        res.errorMessage = "Ví " + wallet->getName() + " đang âm " + formatMoney(-wallet->getBalance()) +
                           " đ. Hãy thêm khoản thu hoặc chỉnh số dư ban đầu trước khi chi.";
        return res;
    }

    // Khoản Chi: Mô phỏng lịch sử
    long long initBal = getWalletInitialBalance(wallet->getId(), wallet->getBalance());
    LinkedList<Transaction*> simList;
    for (const Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data && cur->data->getWalletId() == wallet->getId()) {
            simList.pushBack(cur->data);
        }
    }
    simList.pushBack(const_cast<Transaction*>(tx));
    BalanceValidationResult simRes = validateWalletTimeline(wallet->getId(), initBal, simList, wallet->getName());
    if (!simRes.isValid) {
        simRes.walletName = wallet->getName();
        simRes.requiredAmount = tx->getAmount();
        if (simRes.violationDate == tx->getDate()) {
            simRes.errorCode = ValidationErrorCode::INSUFFICIENT_FUNDS;
            simRes.errorMessage = "Số dư ví " + wallet->getName() + " không đủ: hiện có " +
                                  formatMoney(simRes.availableBalance) + " đ, cần " +
                                  formatMoney(tx->getAmount()) + " đ, thiếu " +
                                  formatMoney(simRes.shortfall) + " đ.";
        } else {
            simRes.errorCode = ValidationErrorCode::WOULD_GO_NEGATIVE_ON_DATE;
            simRes.errorMessage = "Thao tác này làm ví " + wallet->getName() + " âm " +
                                  formatMoney(simRes.shortfall) + " đ vào ngày " +
                                  simRes.violationDate.toString() + ".";
        }
    }
    return simRes;
}

BalanceValidationResult TransactionManager::validateUpdate(int id, long long newAmount, const Date& newDate,
                                                           int newWalletId, const Wallet* oldWallet, const Wallet* newWallet,
                                                           const Category* newCategory) const {
    BalanceValidationResult res;
    const Transaction* oldTx = findById(id);
    if (!oldTx || !oldWallet || !newWallet || !newCategory) {
        res.isValid = false;
        res.errorMessage = "Dữ liệu sửa giao dịch không hợp lệ.";
        return res;
    }
    if (newAmount <= 0) {
        res.isValid = false;
        res.errorCode = ValidationErrorCode::INVALID_AMOUNT;
        res.errorMessage = "Số tiền phải lớn hơn 0.";
        return res;
    }
    if (!newDate.isValid()) {
        res.isValid = false;
        res.errorMessage = "Ngày sửa không hợp lệ.";
        return res;
    }

    // Xử lý ví đang âm:
    if (oldWallet->getId() == newWalletId && oldWallet->getBalance() < 0) {
        if (oldTx->getType() == TransactionType::EXPENSE && newAmount > oldTx->getAmount()) {
            res.isValid = false;
            res.errorCode = ValidationErrorCode::WALLET_ALREADY_NEGATIVE;
            res.errorMessage = "Ví " + oldWallet->getName() + " đang âm " +
                               formatMoney(-oldWallet->getBalance()) +
                               " đ. Hãy thêm khoản thu hoặc chỉnh số dư ban đầu trước khi chi.";
            return res;
        }
        if (oldTx->getType() == TransactionType::INCOME && newAmount < oldTx->getAmount()) {
            res.isValid = false;
            res.errorCode = ValidationErrorCode::WALLET_ALREADY_NEGATIVE;
            res.errorMessage = "Ví " + oldWallet->getName() + " đang âm " +
                               formatMoney(-oldWallet->getBalance()) +
                               " đ. Hãy thêm khoản thu hoặc chỉnh số dư ban đầu trước khi chi.";
            return res;
        }
    }

    Expense tempExpense(id, newAmount, newDate, "", newWalletId, newCategory->getId());
    Income tempIncome(id, newAmount, newDate, "", newWalletId, newCategory->getId());
    Transaction* tempTx = (oldTx->getType() == TransactionType::INCOME)
                        ? static_cast<Transaction*>(&tempIncome)
                        : static_cast<Transaction*>(&tempExpense);

    if (oldWallet->getId() == newWallet->getId()) {
        long long initBal = getWalletInitialBalance(oldWallet->getId(), oldWallet->getBalance());
        LinkedList<Transaction*> simList;
        for (const Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
            if (cur->data && cur->data->getWalletId() == oldWallet->getId()) {
                if (cur->data->getId() == id) {
                    simList.pushBack(tempTx);
                } else {
                    simList.pushBack(cur->data);
                }
            }
        }
        BalanceValidationResult simRes = validateWalletTimeline(oldWallet->getId(), initBal, simList, oldWallet->getName());
        if (!simRes.isValid) {
            simRes.walletName = oldWallet->getName();
            simRes.requiredAmount = newAmount;
            if (simRes.violationDate == newDate) {
                simRes.errorCode = ValidationErrorCode::INSUFFICIENT_FUNDS;
                simRes.errorMessage = "Số dư ví " + oldWallet->getName() + " không đủ: hiện có " +
                                      formatMoney(simRes.availableBalance) + " đ, cần " +
                                      formatMoney(newAmount) + " đ, thiếu " +
                                      formatMoney(simRes.shortfall) + " đ.";
            }
        }
        return simRes;
    } else {
        // Đổi sang ví khác:
        if (newWallet->getBalance() < 0 && tempTx->getType() == TransactionType::EXPENSE) {
            res.isValid = false;
            res.errorCode = ValidationErrorCode::WALLET_ALREADY_NEGATIVE;
            res.errorMessage = "Ví " + newWallet->getName() + " đang âm " +
                               formatMoney(-newWallet->getBalance()) +
                               " đ. Hãy thêm khoản thu hoặc chỉnh số dư ban đầu trước khi chi.";
            return res;
        }

        // 1. Kiểm tra ví cũ khi bỏ oldTx:
        if (oldTx->getType() == TransactionType::INCOME) {
            long long initBalOld = getWalletInitialBalance(oldWallet->getId(), oldWallet->getBalance());
            LinkedList<Transaction*> simOld;
            for (const Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
                if (cur->data && cur->data->getWalletId() == oldWallet->getId() && cur->data->getId() != id) {
                    simOld.pushBack(cur->data);
                }
            }
            res = validateWalletTimeline(oldWallet->getId(), initBalOld, simOld, oldWallet->getName());
            if (!res.isValid) return res;
        }
        // 2. Kiểm tra ví mới khi nhận tempTx:
        if (tempTx->getType() == TransactionType::EXPENSE) {
            long long initBalNew = getWalletInitialBalance(newWallet->getId(), newWallet->getBalance());
            LinkedList<Transaction*> simNew;
            for (const Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
                if (cur->data && cur->data->getWalletId() == newWallet->getId()) {
                    simNew.pushBack(cur->data);
                }
            }
            simNew.pushBack(tempTx);
            res = validateWalletTimeline(newWallet->getId(), initBalNew, simNew, newWallet->getName());
            if (!res.isValid) {
                res.walletName = newWallet->getName();
                res.requiredAmount = newAmount;
                if (res.violationDate == newDate) {
                    res.errorCode = ValidationErrorCode::INSUFFICIENT_FUNDS;
                    res.errorMessage = "Số dư ví " + newWallet->getName() + " không đủ: hiện có " +
                                          formatMoney(res.availableBalance) + " đ, cần " +
                                          formatMoney(newAmount) + " đ, thiếu " +
                                          formatMoney(res.shortfall) + " đ.";
                }
                return res;
            }
        }
    }

    res.isValid = true;
    res.errorCode = ValidationErrorCode::NONE;
    return res;
}

BalanceValidationResult TransactionManager::validateDelete(int id, const Wallet* wallet) const {
    BalanceValidationResult res;
    const Transaction* tx = findById(id);
    if (!tx || !wallet) {
        res.isValid = false;
        res.errorMessage = "Không tìm thấy giao dịch hoặc ví để xóa.";
        return res;
    }
    // Xóa khoản chi là cộng lại tiền cho ví, làm tăng số dư (kể cả ví đang âm giúp giảm âm)
    if (tx->getType() == TransactionType::EXPENSE) {
        res.isValid = true;
        res.errorCode = ValidationErrorCode::NONE;
        res.availableBalance = wallet->getBalance();
        return res;
    }
    // Nếu ví đang âm mà xóa khoản thu -> làm âm thêm -> CHẶN
    if (wallet->getBalance() < 0) {
        res.isValid = false;
        res.errorCode = ValidationErrorCode::WALLET_ALREADY_NEGATIVE;
        res.errorMessage = "Ví " + wallet->getName() + " đang âm " +
                           formatMoney(-wallet->getBalance()) +
                           " đ. Hãy thêm khoản thu hoặc chỉnh số dư ban đầu trước khi chi.";
        return res;
    }
    // Xóa khoản thu: trừ tiền khỏi ví, phải kiểm tra các mốc sau ngày đó
    long long initBal = getWalletInitialBalance(wallet->getId(), wallet->getBalance());
    LinkedList<Transaction*> simList;
    for (const Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data && cur->data->getWalletId() == wallet->getId() && cur->data->getId() != id) {
            simList.pushBack(cur->data);
        }
    }
    res = validateWalletTimeline(wallet->getId(), initBal, simList, wallet->getName());
    if (!res.isValid) {
        res.errorCode = ValidationErrorCode::WOULD_GO_NEGATIVE_ON_DATE;
        res.errorMessage = "Thao tác này làm ví " + wallet->getName() +
                           " âm " + formatMoney(res.shortfall) + " đ vào ngày " +
                           res.violationDate.toString() + ".";
    }
    return res;
}

bool TransactionManager::addTransaction(Transaction* tx, Wallet* wallet, const Category* category) {
    if (!tx || !wallet || !category) return false;
    if (tx->getAmount() <= 0) return false;
    if (!tx->getDate().isValid()) return false;
    if (wallet->getId() != tx->getWalletId()) return false;
    if (category->getId() != tx->getCategoryId()) return false;

    // Kiểm tra loại giao dịch phải khớp với loại của danh mục
    bool isIncomeTx = (tx->getType() == TransactionType::INCOME);
    bool isIncomeCat = (category->getType() == CategoryType::INCOME);
    if (isIncomeTx != isIncomeCat) return false;

    // KIỂM TRA BẤT BIẾN NGHIỆP VỤ: SỐ DƯ VÍ LUÔN >= 0 TRÊN MỌI MỐC LỊCH SỬ
    BalanceValidationResult valRes = validateAdd(tx, wallet);
    if (!valRes.isValid) return false;

    // Cập nhật số dư ví
    applyToWallet(tx, wallet, false);

    transactions.pushBack(tx);
    return true;
}

bool TransactionManager::addLoadedTransaction(Transaction* tx) {
    if (!tx) return false;
    transactions.pushBack(tx);
    return true;
}

void TransactionManager::applyUpdatedFields(Transaction* tx, long long amount, const Date& date,
                                             const std::string& note, int walletId, int categoryId,
                                             Wallet* oldWallet, Wallet* newWallet) {
    applyToWallet(tx, oldWallet, true);
    tx->setAmount(amount);
    tx->setDate(date);
    tx->setNote(note);
    tx->setWalletId(walletId);
    tx->setCategoryId(categoryId);
    applyToWallet(tx, newWallet, false);
}

bool TransactionManager::updateTransaction(int id, long long newAmount, const Date& newDate,
                                           const std::string& newNote, int newWalletId, int newCategoryId,
                                           Wallet* oldWallet, Wallet* newWallet, const Category* newCategory) {
    if (newAmount <= 0 || !newDate.isValid()) return false;
    if (!oldWallet || !newWallet || !newCategory) return false;
    if (newWallet->getId() != newWalletId || newCategory->getId() != newCategoryId) return false;

    Transaction* tx = findById(id);
    if (!tx || tx->getWalletId() != oldWallet->getId()) return false;

    // Kiểm tra loại giao dịch khớp loại danh mục mới
    bool isIncomeTx = (tx->getType() == TransactionType::INCOME);
    bool isIncomeCat = (newCategory->getType() == CategoryType::INCOME);
    if (isIncomeTx != isIncomeCat) return false;

    // KIỂM TRA BẤT BIẾN NGHIỆP VỤ: SỐ DƯ VÍ LUÔN >= 0 TRÊN MỌI MỐC LỊCH SỬ
    BalanceValidationResult valRes = validateUpdate(id, newAmount, newDate, newWalletId, oldWallet, newWallet, newCategory);
    if (!valRes.isValid) return false;

    applyUpdatedFields(tx, newAmount, newDate, newNote, newWalletId, newCategoryId,
                       oldWallet, newWallet);
    return true;
}

bool TransactionManager::deleteTransaction(int id, Wallet* wallet) {
    Transaction* tx = findById(id);
    if (!tx) return false;
    if (!wallet || wallet->getId() != tx->getWalletId()) return false;

    // KIỂM TRA BẤT BIẾN NGHIỆP VỤ: SỐ DƯ VÍ LUÔN >= 0 TRÊN MỌI MỐC LỊCH SỬ
    BalanceValidationResult valRes = validateDelete(id, wallet);
    if (!valRes.isValid) return false;

    // 1. Đảo ngược số dư ví đúng chiều (Xóa Chi -> cộng lại; Xóa Thu -> trừ đi)
    applyToWallet(tx, wallet, true);

    // 2. Xóa khỏi danh sách và giải phóng vùng nhớ
    return transactions.removeIf([id](Transaction* item) {
        if (item && item->getId() == id) {
            delete item;
            return true;
        }
        return false;
    });
}

Transaction* TransactionManager::findById(int id) {
    for (Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data && cur->data->getId() == id) {
            return cur->data;
        }
    }
    return nullptr;
}

const Transaction* TransactionManager::findById(int id) const {
    for (Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data && cur->data->getId() == id) {
            return cur->data;
        }
    }
    return nullptr;
}

bool TransactionManager::isWalletUsed(int walletId) const {
    for (Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data && cur->data->getWalletId() == walletId) {
            return true;
        }
    }
    return false;
}

bool TransactionManager::isCategoryUsed(int categoryId) const {
    for (Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
        if (cur->data && cur->data->getCategoryId() == categoryId) {
            return true;
        }
    }
    return false;
}

size_t TransactionManager::getCount() const {
    return transactions.getSize();
}

// Ghi chú về Unicode: std::tolower của C++ chuẩn chỉ hỗ trợ ký tự ASCII (a-z).
// Với tiếng Việt có dấu, tầng Core hỗ trợ tìm kiếm khớp chính xác hoặc ASCII không dấu;
// tầng Giao diện (Qt) sẽ sử dụng QString::contains(..., Qt::CaseInsensitive) để tìm kiếm
// không phân biệt hoa thường toàn diện cho 100% tiếng Việt có dấu.
static bool containsIgnoreCase(const std::string& source, const std::string& target) {
    if (target.empty()) return true;
    if (source.size() < target.size()) return false;
    std::string sLower = source;
    std::string tLower = target;
    for (char& c : sLower) c = std::tolower(static_cast<unsigned char>(c));
    for (char& c : tLower) c = std::tolower(static_cast<unsigned char>(c));
    return sLower.find(tLower) != std::string::npos;
}

static bool matchesFilter(const Transaction* tx, const Date& fromDate, const Date& toDate,
                          int walletId, int categoryId, int type, const std::string& keyword) {
    if (!tx) return false;
    if (fromDate.isValid() && tx->getDate() < fromDate) return false;
    if (toDate.isValid() && toDate < tx->getDate()) return false;
    if (walletId > 0 && tx->getWalletId() != walletId) return false;
    if (categoryId > 0 && tx->getCategoryId() != categoryId) return false;
    if (type == 1 && tx->getType() != TransactionType::INCOME) return false;
    if (type == 2 && tx->getType() != TransactionType::EXPENSE) return false;
    return keyword.empty() || containsIgnoreCase(tx->getNote(), keyword);
}

// Bộ so sánh tách riêng để thấy rõ chiều sắp xếp khi học Selection Sort.
static bool ngayTangDan(Transaction* a, Transaction* b) {
    return a && b && a->getDate() < b->getDate();
}

static bool ngayGiamDan(Transaction* a, Transaction* b) {
    return a && b && b->getDate() < a->getDate();
}

static bool tienTangDan(Transaction* a, Transaction* b) {
    return a && b && a->getAmount() < b->getAmount();
}

static bool tienGiamDan(Transaction* a, Transaction* b) {
    return a && b && b->getAmount() < a->getAmount();
}

void TransactionManager::getAllTransactions(LinkedList<Transaction*>& outResult) const {
    filterTransactions(outResult);
}

void TransactionManager::filterTransactions(LinkedList<Transaction*>& outResult,
                                            const Date& fromDate,
                                            const Date& toDate,
                                            int walletId,
                                            int categoryId,
                                            int type,
                                            const std::string& keyword) const {
    outResult.clear();
    for (Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
        Transaction* tx = cur->data;
        if (!matchesFilter(tx, fromDate, toDate, walletId, categoryId, type, keyword)) continue;
        outResult.pushBack(tx);
    }
}

void TransactionManager::sortTransactionsByDate(bool ascending) {
    sortByDate(transactions, ascending);
}

void TransactionManager::sortTransactionsByAmount(bool ascending) {
    sortByAmount(transactions, ascending);
}

void TransactionManager::sortByDate(LinkedList<Transaction*>& list, bool ascending) {
    list.selectionSort(ascending ? ngayTangDan : ngayGiamDan);
}

void TransactionManager::sortByAmount(LinkedList<Transaction*>& list, bool ascending) {
    list.selectionSort(ascending ? tienTangDan : tienGiamDan);
}

void TransactionManager::clear() {
    for (Node<Transaction*>* cur = transactions.getHead(); cur != nullptr; cur = cur->next) {
        delete cur->data;
    }
    transactions.clear();
}

LinkedList<Transaction*>& TransactionManager::getTransactions() {
    return transactions;
}

const LinkedList<Transaction*>& TransactionManager::getTransactions() const {
    return transactions;
}
