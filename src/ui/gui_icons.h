#pragma once

#include <QPixmap>
#include <QIcon>
#include <QColor>
#include <QString>

// Bộ icon và đồ họa vẽ tay cho trang Giao dịch và các màn hình khác
namespace GuiIcons {

// Icon tiêu đề trang giao dịch: nền xanh pastel bo góc với hai mũi tên ngược chiều ⇄
QPixmap createExchangeBadge(int size = 44);

// Icon nút Đặt lại: mũi tên xoay tròn
QPixmap createResetIcon(int size = 16, const QColor& color = QColor("#1E6FFF"));

// Mini sparkline hình sóng cho thẻ Tổng thu và Tổng chi
QPixmap createWaveSparkline(bool income, int width = 80, int height = 36);

// Mini chart 4 cột cho thẻ Số giao dịch
QPixmap createMiniBars(int width = 80, int height = 36, const QColor& color = QColor("#1E6FFF"));

// Badge vuông bo góc cho 3 thẻ KPI (income, expense, count)
QPixmap createKpiBadge(const QString& kind, int size = 44);

// Icon tiêu đề trang Ví tiền: nền xanh pastel bo góc 44px
QPixmap createWalletPageHeaderIcon(int size = 44);

// Minh họa ví tiền + xu vàng cho Card Hero tổng tài sản
QPixmap createHeroWalletIllustration(int width = 130, int height = 95);

// Nút chevron tròn > ở góc dưới mỗi thẻ ví
QPixmap createChevronCircle(int size = 32);

// Icon badge bo góc cho từng thẻ ví (Tiền mặt, Techcombank, MoMo, TP BANK...)
QPixmap createWalletCardIcon(const QString& walletName, int walletType, int size = 42);

// Icon tiêu đề trang Thống kê: biểu đồ cột
QPixmap createStatisticsHeaderIcon(int size = 44);

// Cán cân công lý cho thẻ KPI Chênh lệch thu chi
QPixmap createScaleBadge(int size = 44);

// Các icon cho trang Cài đặt (User, Khiên bảo mật, Ổ khóa, Đăng xuất)
QPixmap createUserBadge(int size = 32);
QPixmap createShieldBadge(int size = 32);
QPixmap createLockBadge(int size = 32);
QPixmap createLogoutIcon(int size = 16, const QColor& color = QColor("#E0173D"));
QPixmap createLockActionIcon(int size = 16, const QColor& color = Qt::white);
QPixmap createEyeActionIcon(bool slash = false, int size = 18, const QColor& color = QColor("#5B6B8C"));

// Icon cho các hộp thoại (Dialogs)
QPixmap createCalendarBadge(int size = 44);
QPixmap createPlusCircleBadge(int size = 28);
QPixmap createListIcon(int size = 16, const QColor& color = QColor("#1E6FFF"));

// Icon nhỏ kèm tiêu đề header bảng giao dịch
QIcon createSortHeaderIcon();
QIcon createCoinHeaderIcon();
QIcon createCalendarHeaderIcon();
QIcon createWalletHeaderIcon();
QIcon createTagHeaderIcon();
QIcon createNoteHeaderIcon();

// Icon badge nhỏ cho cột Ví (nền vuông bo góc pastel + icon)
QPixmap createWalletBadgeIcon(const QString& walletName, int walletType, int size = 26);

} // namespace GuiIcons
