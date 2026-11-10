#include "add_wallet_card.h"
#include <QPainter>

AddWalletCard::AddWalletCard(QWidget* parent) : QPushButton("+ Thêm ví", parent) {
    setObjectName("addWalletCard");
    setMinimumSize(260, 176);
    setMaximumHeight(176);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover);
}

void AddWalletCard::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor(underMouse() || isDown() ? "#E8F2FF" : "#FFFFFF"));
    p.setPen(QPen(QColor("#1677FF"), 1.5, Qt::DashLine));
    p.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 15, 15);
    QPoint center(width() / 2, height() / 2 - 16);
    p.setBrush(QColor("#E8F2FF"));
    p.setPen(Qt::NoPen);
    p.drawEllipse(center, 23, 23);
    p.setPen(QPen(QColor("#1677FF"), 2.5, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(center.x() - 9, center.y(), center.x() + 9, center.y());
    p.drawLine(center.x(), center.y() - 9, center.x(), center.y() + 9);
    QFont font = p.font(); font.setPixelSize(15); font.setBold(true); p.setFont(font);
    p.drawText(QRect(0, center.y() + 31, width(), 25), Qt::AlignCenter, "+ Thêm ví");
}
