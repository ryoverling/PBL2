#pragma once

#include <QIcon>
#include <QPixmap>
#include <QColor>
#include <QString>

// Nhóm 2: Icon và Đồ họa cho KPI, Sparklines & Bảng dữ liệu
namespace GuiIcons {

// Sparkline & Mini Chart
QPixmap createSparkline(bool up, int width = 56, int height = 26);
QPixmap createWaveSparkline(bool income, int width = 80, int height = 36);
QPixmap createMiniBars(int width = 80, int height = 36, const QColor& color = QColor("#1E6FFF"));

// Badge vuông bo góc cho KPI & Hộp thoại
QPixmap createKpiBadge(const QString& kind, int size = 44);
QPixmap createScaleBadge(int size = 44);
QPixmap createCalendarBadge(int size = 44);
QPixmap createPlanBadge(const QString& type, int size = 24);

// Icon tròn nền màu cho bảng giao dịch
QIcon createCategoryCircleIcon(const QString& categoryName, int size = 28);

// Icon nhỏ cho Header cột bảng giao dịch
QIcon createSortHeaderIcon();
QIcon createCoinHeaderIcon();
QIcon createCalendarHeaderIcon();
QIcon createWalletHeaderIcon();
QIcon createTagHeaderIcon();
QIcon createNoteHeaderIcon();

} // namespace GuiIcons

