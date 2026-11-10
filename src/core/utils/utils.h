#pragma once

#include <string>
#include "../structure/linked_list.h"

// Forward declarations
class Transaction;
class Category;

// ============================================================================
// Cấu trúc ngày tháng năm đơn giản
// ============================================================================
struct Date {
    int day;
    int month;
    int year;

    // Khởi tạo mặc định
    Date();
    // Khởi tạo có tham số
    Date(int d, int m, int y);

    // Kiểm tra ngày hợp lệ (tháng 1-12, năm > 0, số ngày đúng theo tháng và năm nhuận)
    bool isValid() const;

    // Số ngày trong một tháng (xét cả năm nhuận)
    static int daysInMonth(int m, int y);

    // Thứ trong tuần của ngày 1 trong tháng (0 = Thứ 2, ..., 6 = Chủ Nhật)
    static int weekdayOfFirstDay(int m, int y);

    // Chuyển sang chuỗi hiển thị DD/MM/YYYY (ví dụ: 05/10/2026)
    std::string toString() const;

    // Chuyển sang chuỗi lưu file YYYY-MM-DD (ví dụ: 2026-10-05)
    std::string toFileString() const;

    // Đọc từ chuỗi (hỗ trợ cả "DD/MM/YYYY" lẫn "YYYY-MM-DD")
    static Date fromString(const std::string& str);

    // Lấy ngày hiện tại trên hệ thống
    static Date today();

    // Các toán tử so sánh thứ tự thời gian
    bool operator<(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>=(const Date& other) const;
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
};

// ============================================================================
// Tiện ích định dạng tiền tệ
// ============================================================================
// Định dạng số nguyên thành chuỗi có dấu chấm phân cách hàng nghìn (ví dụ: 1500000 -> "1.500.000")
std::string formatMoney(long long amount);

// Đọc chuỗi tiền tệ (bỏ qua dấu chấm hoặc phẩy) về số nguyên long long
long long parseMoney(const std::string& str);

// Định dạng tiền tệ viết gọn cho biểu đồ/lịch (ví dụ: 60000 -> "60k", 1000000 -> "1.0M")
std::string formatShortMoney(long long amount);

// ============================================================================
// Cấu trúc và tiện ích thống kê
// ============================================================================
// Cấu trúc dữ liệu thống kê tổng thu/chi cho một tháng
struct MonthStat {
    int month = 0;
    int year = 0;
    long long totalIncome = 0;
    long long totalExpense = 0;
};

// Cấu trúc thống kê chi tiêu theo danh mục
struct CategoryExpense {
    std::string categoryName;
    long long totalExpense = 0;
    int percentage = 0; // Tỷ trọng phần trăm làm tròn số nguyên (0 - 100)
};

// Cấu trúc kết quả so sánh chi tiêu với tháng trước
struct ExpenseComparison {
    long long diffAmount = 0;    // Chênh lệch: Tháng này - Tháng trước
    double diffPercent = 0.0;    // Chỉ có nghĩa khi canCompare == true
    bool canCompare = false;     // false nếu tháng trước không có chi (prevExpense == 0)
    std::string message = "không so sánh được"; // Hoặc "Tăng"/"Giảm"/"Không đổi"
};

// Cấu trúc danh sách 31 phần tử tổng chi theo ngày (chỉ số 0 ứng với ngày 1)
struct DailyExpenseList {
    long long days[31];
    DailyExpenseList() {
        for (int i = 0; i < 31; ++i) days[i] = 0;
    }
    long long& operator[](int idx) { return days[idx]; }
    const long long& operator[](int idx) const { return days[idx]; }
};

struct TransactionTotals {
    long long income = 0;
    long long expense = 0;
    int count = 0;
};

// Các hàm tiện ích thống kê thuần túy
namespace StatUtils {
    // Một điều kiện tháng/năm dùng chung cho mọi phép thống kê giao dịch.
    bool cungThangNam(const Transaction* giaoDich, int thang, int nam);
    TransactionTotals sumByType(const LinkedList<Transaction*>& transactions);
    // Tính tổng thu và tổng chi trong một tháng từ TOÀN BỘ giao dịch
    void getMonthlyTotals(int month, int year, const LinkedList<Transaction*>& transactions,
                          long long& outIncome, long long& outExpense);

    // Tính tổng chi tiêu từng ngày trong tháng, trả về mảng/danh sách 31 phần tử (chỉ số 0 ứng với ngày 1)
    DailyExpenseList getExpenseByDay(const LinkedList<Transaction*>& all, int month, int year);

    // Lấy thống kê 6 tháng liên tiếp tính ngược từ (endMonth, endYear)
    void get6MonthsStats(int endMonth, int endYear, const LinkedList<Transaction*>& transactions,
                         MonthStat outStats[6]);

    // Sắp giảm dần theo tổng chi; làm tròn từng tỷ trọng, không phân bổ phần dư.
    // Không có chi: danh sách rỗng; mẫu số bằng 0: tỷ trọng bằng 0%.
    void getExpenseByCategory(int month, int year,
                              const LinkedList<Transaction*>& transactions,
                              const LinkedList<Category>& categories,
                              LinkedList<CategoryExpense>& outList);

    // So sánh tổng chi tiêu của tháng với tháng trước liền kề
    ExpenseComparison compareExpenseWithPreviousMonth(int month, int year,
                                                      const LinkedList<Transaction*>& transactions);
}
