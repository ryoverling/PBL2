#pragma once

#include <QString>
#include <QColor>
#include <QWidget>
#include <QIcon>
#include <QPixmap>
#include "../core/utils/utils.h"
#include "gui_icons_nav.h"
#include "gui_icons_kpi.h"
#include "gui_icons_wallet.h"
#include <initializer_list>

struct BalanceValidationResult;

// Các hàm tiện ích thuần túy cho giao diện Qt (chuẩn sinh viên năm 2)
namespace UiHelper {
    // Cờ trạng thái ẩn/hiện số dư
    extern bool balanceHidden;

    // Định dạng số dư (trả về 8 ký tự • khi ẩn, ngược lại định dạng tiền tệ)
    QString formatBalance(long long amount);

    // Định dạng tiền tệ hiển thị (ví dụ: 1.500.000 đ)
    QString formatMoneyVND(long long amount);

    // Định dạng ngày tháng hiển thị (DD/MM/YYYY)
    QString formatDate(const Date& date);

    // Hiển thị hộp thoại lỗi kiểm tra số dư ví
    void showBalanceError(QWidget* parent, const BalanceValidationResult& res);

    // Hiển thị hộp thoại thông báo lỗi (restyle QMessageBox)
    void showError(QWidget* parent, const QString& message);

    // Hiển thị hộp thoại thông báo thông tin (restyle QMessageBox)
    void showInfo(QWidget* parent, const QString& message);

    // Hộp thoại xác nhận với nút Đồng ý nổi bật và Hủy là nút phụ
    bool askConfirmation(QWidget* parent, const QString& title, const QString& message);

    // Chuẩn hóa tên: cắt khoảng trắng đầu/cuối, gộp khoảng trắng thừa
    QString cleanName(const QString& input);

    // Kiểm tra tên hợp lệ
    bool isValidName(const QString& name);

    // Thiết lập màu thanh tiêu đề hệ thống Windows đồng bộ màu xanh #1E6FFF
    void setupWindowTheme(QWidget* widget);

    // Gắn hiệu ứng đổ bóng mềm cho thẻ Card
    void applyCardShadow(QWidget* target, int blurRadius = 14, int offsetY = 2, qreal opacity = 0.08);
    void applyCardShadows(const std::initializer_list<QWidget*>& targets, int blurRadius = 14, int offsetY = 2, qreal opacity = 0.08);

    // Bảng màu chuẩn hệ thống MoneyManager
    QColor colorPrimary();    // #1E6FFF
    QColor colorNavy();       // #0B2A6B
    QColor colorIncome();     // #0E8F4F
    QColor colorExpense();    // #E0173D
    QColor colorIncomeBg();   // #EAFBF3 (nền nhạt thu nhập)
    QColor colorExpenseBg();  // #FFF0F3 (nền nhạt chi tiêu)

    // Biểu tượng chuỗi danh mục và ví tiền
    QString getCategoryIcon(const QString& categoryName);
    QString getWalletIcon(const QString& walletName, int walletType = 0);

    // Chuyển tiếp icon sang GuiIcons để tương thích mã gọi cũ
    using GuiIcons::createAppLogoIcon;
    using GuiIcons::createHomeNavIcon;
    using GuiIcons::createExchangeNavIcon;
    using GuiIcons::createWalletNavIcon;
    using GuiIcons::createStatsNavIcon;
    using GuiIcons::createSettingsNavIcon;
    using GuiIcons::createUserIcon;
    using GuiIcons::createLockIcon;
    using GuiIcons::createRightArrowIcon;
    using GuiIcons::createKeyIcon;
    using GuiIcons::createLoginIllustration;
    using GuiIcons::createPlusIcon;
    using GuiIcons::createEditIcon;
    using GuiIcons::createTrashIcon;
    using GuiIcons::createCalendarIcon;
    using GuiIcons::createFilterIcon;
    using GuiIcons::createWalletIcon;
    using GuiIcons::createTagIcon;
    using GuiIcons::createArrowUpIcon;
    using GuiIcons::createArrowDownIcon;
    using GuiIcons::createSparkline;
    using GuiIcons::createCategoryCircleIcon;
    using GuiIcons::createPlanBadge;
}
