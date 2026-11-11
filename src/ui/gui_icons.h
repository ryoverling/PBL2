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
