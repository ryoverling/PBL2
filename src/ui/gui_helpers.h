#pragma once

#include <QString>
#include <QColor>
#include <QWidget>
#include "../core/utils/utils.h"

// Các hàm tiện ích dùng chung cho tầng giao diện Qt
namespace UiHelper {
    // Cờ trạng thái ẩn/hiện số dư
    extern bool balanceHidden;

    // Định dạng số dư (trả về 8 ký tự • khi ẩn, ngược lại định dạng tiền tệ)
    QString formatBalance(long long amount);

    // Định dạng tiền tệ hiển thị (ví dụ: 1.500.000 đ)
    QString formatMoneyVND(long long amount);

    // Định dạng ngày tháng hiển thị (DD/MM/YYYY)
    QString formatDate(const Date& date);

    // Hiển thị hộp thoại thông báo lỗi ngắn gọn
    void showError(QWidget* parent, const QString& message);

    // Hiển thị hộp thoại thông báo thông tin
    void showInfo(QWidget* parent, const QString& message);

    // Xác nhận với nút Đồng ý nổi bật và Hủy là nút phụ
    bool askConfirmation(QWidget* parent, const QString& title, const QString& message);

    // Chuẩn hóa tên: cắt khoảng trắng đầu/cuối, gộp khoảng trắng thừa
    QString cleanName(const QString& input);

    // Kiểm tra tên hợp lệ (không rỗng, không chứa ký tự phân cách | \n \r)
    bool isValidName(const QString& name);

    // Thiết lập màu thanh tiêu đề Windows đồng bộ giao diện
    void setupWindowTheme(QWidget* widget);

    // Tạo các Icon vector chuẩn Design System cho nút bấm
    class QIcon createPlusIcon(const QColor& color = Qt::white);
    class QIcon createEditIcon(const QColor& color = QColor("#1769EE"));
    class QIcon createTrashIcon(const QColor& color = QColor("#EF4765"));
    class QIcon createListIcon(const QColor& color = QColor("#1769EE"));
    class QIcon createCalendarIcon(const QColor& color = QColor("#1769EE"));
    class QIcon createSearchIcon(const QColor& color = QColor("#64748B"));
    class QIcon createFilterIcon(const QColor& color = QColor("#64748B"));
    class QIcon createWalletIcon(const QColor& color = QColor("#64748B"));
    class QIcon createTagIcon(const QColor& color = QColor("#64748B"));
    class QIcon createArrowUpIcon(const QColor& color = QColor("#10B981"));
    class QIcon createArrowDownIcon(const QColor& color = QColor("#EF4444"));

    // Biểu tượng danh mục và ví tiền hợp lý
    QString getCategoryIcon(const QString& categoryName);
    QString getWalletIcon(const QString& walletName, int walletType = 0);

    // Màu sắc quy ước chuẩn
    QColor colorIncome();   // #10B981
    QColor colorExpense();  // #FF405C
}
