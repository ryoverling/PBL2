#pragma once

#include <QPixmap>
#include <QColor>
#include <QString>

// Nhóm 3: Icon và Đồ họa cho Ví tiền, Thống kê, Cài đặt & Hình minh họa
namespace GuiIcons {

// Hình minh họa ví tiền & đăng nhập
QPixmap createHeroWalletIllustration(int width = 130, int height = 95);
QPixmap createLoginIllustration(int width = 200, int height = 120);

// Thẻ ví & header trang Ví
QPixmap createWalletCardIcon(const QString& walletName, int walletType, int size = 42);
QPixmap createWalletBadgeIcon(const QString& walletName, int walletType, int size = 26);
QPixmap createChevronCircle(int size = 32);
QPixmap createWalletPageHeaderIcon(int size = 44);

// Header trang Thống kê & Badge trang Cài đặt
QPixmap createStatisticsHeaderIcon(int size = 44);
QPixmap createUserBadge(int size = 32);
QPixmap createShieldBadge(int size = 32);
QPixmap createLockBadge(int size = 32);

} // namespace GuiIcons

