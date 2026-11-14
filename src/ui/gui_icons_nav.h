#pragma once

#include <QIcon>
#include <QPixmap>
#include <QColor>

// Nhóm 1: Icon Vector cho Navigation Header, Xác thực & Nút thao tác
namespace GuiIcons {

// App Logo & 5 Nav Header Icons
QIcon createAppLogoIcon(int size = 36);
QIcon createHomeNavIcon(const QColor& color = QColor("#5B6B8C"));
QIcon createExchangeNavIcon(const QColor& color = QColor("#5B6B8C"));
QIcon createWalletNavIcon(const QColor& color = QColor("#5B6B8C"));
QIcon createStatsNavIcon(const QColor& color = QColor("#5B6B8C"));
QIcon createSettingsNavIcon(const QColor& color = QColor("#5B6B8C"));

// Icon cho Form Đăng nhập & Đăng ký
QIcon createUserIcon(const QColor& color = QColor("#5B6B8C"));
QIcon createLockIcon(const QColor& color = QColor("#5B6B8C"));
QIcon createRightArrowIcon(const QColor& color = Qt::white);
QIcon createKeyIcon(const QColor& color = QColor("#1E6FFF"));

// Icon cho các Nút thao tác
QIcon createPlusIcon(const QColor& color = Qt::white);
QIcon createEditIcon(const QColor& color = QColor("#1E6FFF"));
QIcon createTrashIcon(const QColor& color = QColor("#E0173D"));
QIcon createListIcon(int size = 16, const QColor& color = QColor("#1E6FFF"));
QIcon createCalendarIcon(const QColor& color = QColor("#1E6FFF"));
QIcon createFilterIcon(const QColor& color = QColor("#5B6B8C"));
QIcon createWalletIcon(const QColor& color = QColor("#5B6B8C"));
QIcon createTagIcon(const QColor& color = QColor("#5B6B8C"));
QIcon createArrowUpIcon(const QColor& color = QColor("#0E8F4F"));
QIcon createArrowDownIcon(const QColor& color = QColor("#E0173D"));

// Nút reset, logout, mắt, khóa, badge tiêu đề giao dịch
QPixmap createResetIcon(int size = 16, const QColor& color = QColor("#1E6FFF"));
QPixmap createLogoutIcon(int size = 16, const QColor& color = QColor("#E0173D"));
QPixmap createLockActionIcon(int size = 16, const QColor& color = Qt::white);
QPixmap createEyeActionIcon(bool slash = false, int size = 18, const QColor& color = QColor("#5B6B8C"));
QPixmap createExchangeBadge(int size = 44);

} // namespace GuiIcons

