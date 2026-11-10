#pragma once

#include <string>
#include "../model/user.h"
#include "../model/wallet.h"
#include "../model/category.h"
#include "../model/transaction.h"
#include "../manager/user_manager.h"
#include "../manager/wallet_manager.h"
#include "../manager/category_manager.h"
#include "../manager/transaction_manager.h"
#include "../utils/stat_utils.h"

// Lớp điều phối MoneyManager (Coordinator)
// Là đầu mối trung tâm xử lý các nghiệp vụ liên-manager:
// Phối hợp giữa Wallet, Category và Transaction;
// Tự tra cứu thực thể, kiểm tra ràng buộc sử dụng và cập nhật số dư.
class MoneyManager {
private:
    UserManager userManager;
    std::string currentUsername;
    WalletManager walletManager;
    CategoryManager categoryManager;
    TransactionManager transactionManager;
public:
    MoneyManager();
    ~MoneyManager() = default;

    // Tắt copy constructor và copy assignment
    MoneyManager(const MoneyManager&) = delete;
    MoneyManager& operator=(const MoneyManager&) = delete;

    // Xóa sạch toàn bộ dữ liệu trong tất cả manager
    void clearAll();

    // Quản lý và xác thực người dùng qua UserManager
    bool loadUsers(const std::string& baseDataDir);
    bool saveUsers(const std::string& baseDataDir) const;

    bool login(const std::string& username, const std::string& password);
    bool registerUser(const std::string& username, const std::string& password, const std::string& name = "");
    bool changePassword(const std::string& username, const std::string& oldPassword, const std::string& newPassword);
    void logout();

    const std::string& getCurrentUsername() const;
    void setCurrentUsername(const std::string& username);

    // Lấy đường dẫn thư mục dữ liệu của người dùng: <baseDataDir>/<username>
    static std::string getUserDataDir(const std::string& baseDataDir, const std::string& username);

    // Đọc / Ghi toàn bộ dữ liệu từ thư mục người dùng (userDir)
    bool loadAll(const std::string& userDir);
    bool saveAll(const std::string& userDir) const;

    // Nghiệp vụ Ví tiền
    bool addWallet(const std::string& name, WalletType type, long long initialBalance, int* outId = nullptr);
    bool updateWallet(int id, const std::string& name, WalletType type);
    bool deleteWallet(int id);

    // Nghiệp vụ Danh mục
    bool addCategory(const std::string& name, CategoryType type, int* outId = nullptr);
    bool updateCategory(int id, const std::string& newName, CategoryType newType);
    bool deleteCategory(int id);

    // Nghiệp vụ Giao dịch: nhận tên danh mục + loại, tự kiểm tra tính hợp lệ trước khi tạo danh mục
    bool addTransaction(Transaction* tx, const std::string& categoryName, CategoryType categoryType);

    bool updateTransaction(int id, long long newAmount, const Date& newDate,
                           const std::string& newNote, int newWalletId,
                           const std::string& newCategoryName, CategoryType newCatType);

    bool deleteTransaction(int id);

    // Kiểm tra tính hợp lệ số dư trước khi thêm / sửa / xóa (cho UI hiển thị cảnh báo chi tiết):
    BalanceValidationResult validateAddTransaction(const Transaction* tx) const;
    BalanceValidationResult validateUpdateTransaction(int id, long long newAmount, const Date& newDate,
                                                      int newWalletId, const std::string& newCategoryName,
                                                      CategoryType newCatType) const;
    BalanceValidationResult validateDeleteTransaction(int id) const;
    long long getWalletAvailableBalance(int walletId) const;

    // Thống kê thu / chi
    void getMonthlyTotals(int month, int year, long long& outIncome, long long& outExpense) const;
    void get6MonthsStats(int endMonth, int endYear, MonthStat outStats[6]) const;
    void getExpenseByCategory(int month, int year, LinkedList<CategoryExpense>& outList) const;
    ExpenseComparison compareExpenseWithPreviousMonth(int month, int year) const;
    DailyExpenseList getExpenseByDay(int month, int year) const;

    // Dữ liệu cho giao diện: lọc và sắp ngày giảm dần trong core.
    bool getFilteredTransactions(LinkedList<Transaction*>& result,
                                 const Date& from = Date(0, 0, 0), const Date& to = Date(0, 0, 0),
                                 int walletId = 0, int categoryId = 0, int type = 0) const;
    long long getMonthlyDifference(int month, int year) const;

    // Getters chỉ dùng để tầng UI đọc danh sách và hiển thị giao diện
    WalletManager& getWalletManager();
    const WalletManager& getWalletManager() const;

    CategoryManager& getCategoryManager();
    const CategoryManager& getCategoryManager() const;

    TransactionManager& getTransactionManager();
    const TransactionManager& getTransactionManager() const;
};
