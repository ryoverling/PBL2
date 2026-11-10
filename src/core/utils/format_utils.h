#pragma once

#include <string>

// Định dạng số nguyên thành chuỗi có dấu chấm phân cách hàng nghìn (ví dụ: 1500000 -> "1.500.000")
std::string formatMoney(long long amount);

// Đọc chuỗi tiền tệ (bỏ qua dấu chấm hoặc phẩy) về số nguyên long long
long long parseMoney(const std::string& str);

// Định dạng tiền tệ viết gọn cho biểu đồ/lịch (ví dụ: 60000 -> "60k", 1000000 -> "1.0M")
std::string formatShortMoney(long long amount);
