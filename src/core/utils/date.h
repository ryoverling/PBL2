#pragma once

#include <string>

// Cấu trúc ngày tháng năm đơn giản
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
