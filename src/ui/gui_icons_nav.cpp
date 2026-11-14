#include "gui_icons_nav.h"
#include <QPainter>

namespace GuiIcons {

QIcon createAppLogoIcon(int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QLinearGradient bg(0, 0, size, size);
    bg.setColorAt(0.0, QColor("#1E6FFF"));
    bg.setColorAt(1.0, QColor("#0B2A6B"));
    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    p.drawRoundedRect(0, 0, size, size, 8, 8);

    p.setBrush(Qt::white);
    int barW = size / 6;
    int r = barW / 2;
    int gap = size / 8;
    int x1 = size / 5;
    int x2 = x1 + barW + gap;
    int x3 = x2 + barW + gap;
    int b = size - size / 5;

    p.drawRoundedRect(x1, b - size / 3, barW, size / 3, r, r);
    p.drawRoundedRect(x2, b - size / 2, barW, size / 2, r, r);
    p.drawRoundedRect(x3, b - (size * 2) / 3, barW, (size * 2) / 3, r, r);
    return QIcon(pix);
}

QIcon createHomeNavIcon(const QColor& color) {
    QPixmap pix(24, 24);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPointF roof[3] = { QPointF(12, 4), QPointF(4, 11), QPointF(20, 11) };
    p.drawPolyline(roof, 3);
    p.drawRect(6, 11, 12, 9);
    p.drawLine(12, 16, 12, 20);
    return QIcon(pix);
}

QIcon createExchangeNavIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawLine(3, 7, 17, 7);
    p.drawLine(13, 3, 17, 7);
    p.drawLine(13, 11, 17, 7);
    p.drawLine(17, 13, 3, 13);
    p.drawLine(7, 9, 3, 13);
    p.drawLine(7, 17, 3, 13);
    return QIcon(pix);
}

QIcon createWalletNavIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawRoundedRect(2, 4, 16, 12, 3, 3);
    p.drawLine(2, 8, 18, 8);
    p.setBrush(color);
    p.drawEllipse(13, 10, 2, 2);
    return QIcon(pix);
}

QIcon createStatsNavIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(color);
    p.drawRoundedRect(3, 10, 3, 7, 1, 1);
    p.drawRoundedRect(8, 4, 3, 13, 1, 1);
    p.drawRoundedRect(13, 7, 3, 10, 1, 1);
    return QIcon(pix);
}

QIcon createSettingsNavIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap));
    p.drawEllipse(6, 6, 8, 8);
    p.drawLine(10, 2, 10, 5);
    p.drawLine(10, 15, 10, 18);
    p.drawLine(2, 10, 5, 10);
    p.drawLine(15, 10, 18, 10);
    p.drawLine(4, 4, 6, 6);
    p.drawLine(14, 14, 16, 16);
    p.drawLine(16, 4, 14, 6);
    p.drawLine(4, 16, 6, 14);
    return QIcon(pix);
}

QIcon createUserIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawEllipse(6, 3, 8, 8);
    p.drawArc(3, 12, 14, 10, 0, 180 * 16);
    return QIcon(pix);
}

QIcon createLockIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawRoundedRect(4, 9, 12, 9, 2, 2);
    p.drawArc(6, 3, 8, 8, 0, 180 * 16);
    p.setBrush(color);
    p.drawEllipse(9, 12, 2, 2);
    return QIcon(pix);
}

QIcon createRightArrowIcon(const QColor& color) {
    QPixmap pix(18, 18);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawLine(4, 9, 14, 9);
    p.drawLine(10, 5, 14, 9);
    p.drawLine(10, 13, 14, 9);
    return QIcon(pix);
}

QIcon createKeyIcon(const QColor& color) {
    QPixmap pix(18, 18);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap));
    p.drawEllipse(3, 6, 7, 7);
    p.drawLine(10, 10, 15, 10);
    p.drawLine(13, 10, 13, 13);
    p.drawLine(15, 10, 15, 12);
    return QIcon(pix);
}

QIcon createPlusIcon(const QColor& color) {
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(8, 3, 8, 13);
    p.drawLine(3, 8, 13, 8);
    return QIcon(pix);
}

QIcon createEditIcon(const QColor& color) {
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawLine(3, 13, 13, 3);
    p.drawLine(10, 3, 13, 6);
    p.drawLine(2, 14, 5, 14);
    p.drawLine(2, 14, 2, 11);
    return QIcon(pix);
}

QIcon createTrashIcon(const QColor& color) {
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawLine(3, 4, 13, 4);
    p.drawLine(6, 2, 10, 2);
    p.drawRect(4, 5, 8, 9);
    p.drawLine(6, 7, 6, 12);
    p.drawLine(10, 7, 10, 12);
    return QIcon(pix);
}

QIcon createListIcon(int size, const QColor& color) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap));
    int l = size * 5 / 16;
    int r = size * 14 / 16;
    int d = size / 6;
    p.drawLine(l, size * 4 / 16, r, size * 4 / 16);
    p.drawLine(l, size * 8 / 16, r, size * 8 / 16);
    p.drawLine(l, size * 12 / 16, r, size * 12 / 16);
    p.setBrush(color);
    p.setPen(Qt::NoPen);
    p.drawEllipse(size * 2 / 16, size * 3 / 16, d, d);
    p.drawEllipse(size * 2 / 16, size * 7 / 16, d, d);
    p.drawEllipse(size * 2 / 16, size * 11 / 16, d, d);
    return QIcon(pix);
}

QIcon createCalendarIcon(const QColor& color) {
    QPixmap pix(18, 18);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawRoundedRect(2, 3, 14, 12, 2, 2);
    p.drawLine(2, 7, 16, 7);
    p.drawLine(5, 1, 5, 3);
    p.drawLine(13, 1, 13, 3);
    return QIcon(pix);
}

QIcon createFilterIcon(const QColor& color) {
    QPixmap pix(18, 18);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPointF pts[4] = { QPointF(2, 3), QPointF(16, 3), QPointF(10, 10), QPointF(10, 15) };
    p.drawPolyline(pts, 4);
    return QIcon(pix);
}

QIcon createWalletIcon(const QColor& color) {
    return createWalletNavIcon(color);
}

QIcon createTagIcon(const QColor& color) {
    QPixmap pix(18, 18);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPointF pts[5] = { QPointF(3, 8), QPointF(8, 3), QPointF(15, 3), QPointF(15, 10), QPointF(10, 15) };
    p.drawPolygon(pts, 5);
    p.setBrush(color);
    p.drawEllipse(12, 5, 2, 2);
    return QIcon(pix);
}

QIcon createArrowUpIcon(const QColor& color) {
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawLine(8, 13, 8, 3);
    p.drawLine(4, 7, 8, 3);
    p.drawLine(12, 7, 8, 3);
    return QIcon(pix);
}

QIcon createArrowDownIcon(const QColor& color) {
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawLine(8, 3, 8, 13);
    p.drawLine(4, 9, 8, 13);
    p.drawLine(12, 9, 8, 13);
    return QIcon(pix);
}

QPixmap createResetIcon(int size, const QColor& color) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap));
    QRectF arcRect(2, 2, size - 4, size - 4);
    p.drawArc(arcRect, 45 * 16, 270 * 16);
    p.drawLine(size - 3, 2, size - 3, 7);
    p.drawLine(size - 3, 2, size - 8, 2);
    return pix;
}

QPixmap createLogoutIcon(int size, const QColor& color) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawLine(3, 2, 9, 2);
    p.drawLine(3, 2, 3, size - 2);
    p.drawLine(3, size - 2, 9, size - 2);
    p.drawLine(7, size / 2, size - 2, size / 2);
    p.drawLine(size - 5, size / 2 - 3, size - 2, size / 2);
    p.drawLine(size - 5, size / 2 + 3, size - 2, size / 2);
    return pix;
}

QPixmap createLockActionIcon(int size, const QColor& color) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawRoundedRect(2, size * 7 / 16, size - 4, size * 8 / 16, 2, 2);
    p.drawArc(size * 4 / 16, 2, size * 8 / 16, size * 8 / 16, 0, 180 * 16);
    return pix;
}

QPixmap createEyeActionIcon(bool slash, int size, const QColor& color) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QRectF r(1, size / 4, size - 2, size / 2);
    p.drawArc(r, 20 * 16, 140 * 16);
    p.drawArc(r, 200 * 16, 140 * 16);
    p.setBrush(color);
    p.drawEllipse(QPointF(size / 2.0, size / 2.0), 2.2, 2.2);
    if (slash) {
        p.setPen(QPen(QColor("#E0173D"), 1.8, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(2, size - 2, size - 2, 2);
    }
    return pix;
}

} // namespace GuiIcons

