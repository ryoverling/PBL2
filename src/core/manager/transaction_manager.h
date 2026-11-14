#pragma once

#include <string>
#include "../model/transaction.h"
#include "../model/wallet.h"
#include "../model/category.h"
#include "../structure/linked_list.h"

// Enum mã lỗi chuẩn hóa cho kiểm tra số dư và nghiệp vụ giao dịch
enum class ValidationErrorCode {
    NONE = 0,
    INSUFFICIENT_FUNDS,
    WOULD_GO_NEGATIVE_ON_DATE,
    WALLET_ALREADY_NEGATIVE,
    INVALID_AMOUNT
};

// Kết quả kiểm tra tính toàn vẹn số dư ví
struct BalanceValidationResult {
    bool isValid;
    ValidationErrorCode errorCode;
    long long availableBalance; // Số dư khả dụng
    long long requiredAmount;   // Số tiền muốn chi / ảnh hưởng
    long long shortfall;        // Số tiền còn thiếu nếu không đủ
    Date violationDate;         // Ngày xảy ra số dư âm trong lịch sử
    std::string walletName;     // Tên ví liên quan
    std::string errorMessage;   // Thông báo chi tiết phục vụ người dùng

    BalanceValidationResult()
        : isValid(true), errorCode(ValidationErrorCode::NONE), availableBalance(0), requiredAmount(0),
          shortfall(0), violationDate(Date(0, 0, 0)), walletName(""), errorMessage("") {}
};

// Quản lý danh sách Giao dịch (không phụ thuộc WalletManager hay CategoryManager)
class TransactionManager {
private:
    LinkedList<Transaction*> transactions;

    // Hàm trợ giúp cập nhật số dư ví:
    // - isReverse = false: Áp dụng giao dịch (Thu: +, Chi: -)
    // - isReverse = true:  Đảo tác động khi sửa hoặc xóa (Thu: -, Chi: +)
    static void applyToWallet(const Transaction* tx, Wallet* wallet, bool isReverse);
    static void applyUpdatedFields(Transaction* tx, long long amount, const Date& date,
                                   const std::string& note, int walletId, int categoryId,
                                   Wallet* oldWallet, Wallet* newWallet);

public:
    TransactionManager() = default;
    ~TransactionManager();

    // Tắt copy constructor và copy assignment
    TransactionManager(const TransactionManager&) = delete;
    TransactionManager& operator=(const TransactionManager&) = delete;

    // Sinh ID tự tăng tiếp theo = max(id) + 1, rỗng thì là 1
    int getNextId() const;

    // Tính số dư ban đầu của ví dựa trên số dư hiện tại và toàn bộ lịch sử giao dịch:
    long long getWalletInitialBalance(int walletId, long long currentBalance) const;

    // Kiểm tra mô phỏng xem chuỗi giao dịch của một ví có bao giờ rơi vào số dư âm hay không:
    BalanceValidationResult validateWalletTimeline(
        int walletId,
        long long initialBalance,
        const LinkedList<Transaction*>& simulatedTxList,
        const std::string& walletName = "") const;

    // Các hàm kiểm tra nghiệp vụ độc lập:
    BalanceValidationResult validateAdd(const Transaction* tx, const Wallet* wallet) const;
    BalanceValidationResult validateUpdate(int id, long long newAmount, const Date& newDate,
                                           int newWalletId, const Wallet* oldWallet, const Wallet* newWallet,
                                           const Category* newCategory) const;
    BalanceValidationResult validateDelete(int id, const Wallet* wallet) const;

    // Thêm giao dịch: kiểm tra hợp lệ, kiểm tra không âm ví, cập nhật số dư, lưu danh sách
    bool addTransaction(Transaction* tx, Wallet* wallet, const Category* category);

    // Nạp bản ghi đã lưu mà không cộng trừ số dư ví lần nữa.
    bool addLoadedTransaction(Transaction* tx);

    // Sửa giao dịch: hoàn tiền ví cũ, áp tiền ví mới, cập nhật thông tin
    bool updateTransaction(int id, long long newAmount, const Date& newDate,
                           const std::string& newNote, int newWalletId, int newCategoryId,
                           Wallet* oldWallet, Wallet* newWallet, const Category* newCategory);

    // Xóa giao dịch: đảo ngược số dư ví đúng chiều và giải phóng bộ nhớ
    bool deleteTransaction(int id, Wallet* wallet);

    // Tìm kiếm giao dịch theo ID
    Transaction* findById(int id);
    const Transaction* findById(int id) const;

    // Kiểm tra xem một ví có đang được sử dụng trong giao dịch nào không
    bool isWalletUsed(int walletId) const;

    // Lấy số lượng giao dịch
    size_t getCount() const;

    // LỌC GIAO DỊCH:
    // Đẩy các con trỏ THAM CHIẾU (không sao chép đối tượng) vào outResult.
    // LƯU Ý QUAN TRỌNG: Danh sách outResult KHÔNG sở hữu bộ nhớ và TUYỆT ĐỐI KHÔNG ĐƯỢC delete các phần tử!
    // Các điều kiện kết hợp được với nhau (để trống hoặc 0 nghĩa là không lọc điều kiện đó):
    void filterTransactions(LinkedList<Transaction*>& outResult,
                            const Date& fromDate = Date(0, 0, 0),
                            const Date& toDate = Date(0, 0, 0),
                            int walletId = 0,
                            int categoryId = 0,
                            int type = 0, // 0: Tất cả, 1: Thu nhập, 2: Chi tiêu
                            const std::string& keyword = "") const;

    // SẮP XẾP GIAO DỊCH THEO NGÀY (Sử dụng Selection Sort trên LinkedList):
    // Sắp xếp trên danh sách chính của Manager
    void sortTransactionsByDate(bool ascending = true);

    // Sắp xếp trên bất kỳ danh sách nào (ví dụ danh sách sau khi lọc)
    static void sortByDate(LinkedList<Transaction*>& list, bool ascending = true);

    // Xóa sạch toàn bộ giao dịch và delete từng con trỏ
    void clear();

    // Truy xuất danh sách giao dịch
    LinkedList<Transaction*>& getTransactions();
    const LinkedList<Transaction*>& getTransactions() const;
};
