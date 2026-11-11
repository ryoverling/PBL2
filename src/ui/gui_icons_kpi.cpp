#include "gui_icons_kpi.h"
#include <QPainter>
#include <QPainterPath>

namespace GuiIcons {

QPixmap createSparkline(bool up, int width, int height) {
    QPixmap pix(width, height);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    QColor color = up ? QColor("#0E8F4F") : QColor("#E0173D");
    color.setAlpha(190);
    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPainterPath path;
    if (up) {
        path.moveTo(4, height - 7);
        path.cubicTo(width * 0.3, height - 5, width * 0.4, height * 0.3, width * 0.7, height * 0.45);
        path.cubicTo(width * 0.85, height * 0.55, width * 0.9, height * 0.2, width - 4, 4);
    } else {
        path.moveTo(4, height * 0.45);
        path.cubicTo(width * 0.25, height * 0.65, width * 0.5, height * 0.2, width * 0.75, height * 0.7);
        path.cubicTo(width * 0.85, height * 0.9, width * 0.92, height * 0.35, width - 4, height * 0.5);
    }
    p.drawPath(path);
    return pix;
}

QPixmap createWaveSparkline(bool income, int width, int height) {
    QPixmap pix(width, height);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QColor lineColor = income ? QColor("#0E8F4F") : QColor("#E0173D");
    QColor fillColor = income ? QColor(14, 143, 79, 35) : QColor(224, 23, 61, 35);

    QPainterPath path;
    path.moveTo(4, height * 0.70);
    path.cubicTo(width * 0.35, height * 0.85, width * 0.45, height * 0.20, width * 0.70, height * 0.35);
    path.cubicTo(width * 0.80, height * 0.40, width * 0.90, height * 0.15, width - 4, height * 0.25);

    QPainterPath fillPath = path;
    fillPath.lineTo(width - 4, height);
    fillPath.lineTo(4, height);
    fillPath.closeSubpath();
    p.fillPath(fillPath, fillColor);

    p.setPen(QPen(lineColor, 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPath(path);
    return pix;
}

QPixmap createMiniBars(int width, int height, const QColor& color) {
    QPixmap pix(width, height);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    const qreal heights[4] = {0.35, 0.55, 0.75, 1.00};
    int barW = 8;
    int gap = 5;
    int totalW = 4 * barW + 3 * gap;
    int startX = width - totalW - 4;

    for (int i = 0; i < 4; ++i) {
        int bH = static_cast<int>(height * heights[i]);
        int x = startX + i * (barW + gap);
        int y = height - bH;
        QRect rect(x, y, barW, bH);
        QLinearGradient grad(rect.topLeft(), rect.bottomLeft());
        grad.setColorAt(0.0, color);
        grad.setColorAt(1.0, color.lighter(130));
        p.setPen(Qt::NoPen);
        p.setBrush(grad);
        p.drawRoundedRect(rect, 4, 4);
    }
    return pix;
}

QPixmap createKpiBadge(const QString& kind, int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QColor bg = QColor("#E0EDFF");
    QColor fg = QColor("#1E6FFF");
    if (kind == "income") {
        bg = QColor("#E6FAF3");
        fg = QColor("#0E8F4F");
    } else if (kind == "expense") {
        bg = QColor("#FEECEB");
        fg = QColor("#E0173D");
    }

    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    p.drawRoundedRect(0, 0, size, size, 12, 12);
    p.setPen(QPen(fg, 2.4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    int c = size / 2;
    int r = size * 0.28;
    if (kind == "income") {
        p.drawLine(c - r, c + r, c + r, c - r);
        p.drawLine(c - 2, c - r, c + r, c - r);
        p.drawLine(c + r, c - r, c + r, c + 2);
    } else if (kind == "expense") {
        p.drawLine(c, c - r, c, c + r);
        p.drawLine(c - 5, c + r - 5, c, c + r);
        p.drawLine(c + 5, c + r - 5, c, c + r);
    } else {
        // Biểu tượng các lớp xếp chồng
        p.setPen(QPen(fg, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        QPolygonF dia1;
        dia1 << QPointF(c, c - 7) << QPointF(c + 9, c - 3) << QPointF(c, c + 1) << QPointF(c - 9, c - 3);
        p.drawPolygon(dia1);
        p.drawLine(c - 9, c + 1, c, c + 5);
        p.drawLine(c + 9, c + 1, c, c + 5);
        p.drawLine(c - 9, c + 5, c, c + 9);
        p.drawLine(c + 9, c + 5, c, c + 9);
    }
    return pix;
}

QPixmap createScaleBadge(int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#E0EDFF"));
    p.drawRoundedRect(0, 0, size, size, 12, 12);

    p.setPen(QPen(QColor("#1E6FFF"), 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    int c = size / 2;
    p.drawLine(c, 10, c, size - 10);
    p.drawLine(10, 14, size - 10, 14);

    p.drawLine(10, 14, 6, 24);
    p.drawLine(10, 14, 14, 24);
    p.drawArc(6, 20, 8, 8, 180 * 16, 180 * 16);

    p.drawLine(size - 10, 14, size - 14, 24);
    p.drawLine(size - 10, 14, size - 6, 24);
    p.drawArc(size - 14, 20, 8, 8, 180 * 16, 180 * 16);
    return pix;
}

QPixmap createCalendarBadge(int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#E0EDFF"));
    p.drawRoundedRect(0, 0, size, size, 12, 12);

    int m = size * 0.22;
    int w = size - m * 2;
    int h = size - m * 2;

    p.setPen(QPen(QColor("#1E6FFF"), 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(m, m + 2, w, h - 2, 3, 3);
    p.drawLine(m, m + 8, m + w, m + 8);
    p.drawLine(m + 4, m, m + 4, m + 4);
    p.drawLine(m + w - 4, m, m + w - 4, m + 4);
    p.drawPoint(m + 5, m + 13);
    p.drawPoint(m + 11, m + 13);
    p.drawPoint(m + 17, m + 13);
    return pix;
}

QPixmap createPlusCircleBadge(int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#1E6FFF"));
    p.drawEllipse(0, 0, size, size);

    p.setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap));
    int c = size / 2;
    int r = size * 0.28;
    p.drawLine(c, c - r, c, c + r);
    p.drawLine(c - r, c, c + r, c);
    return pix;
}

QPixmap createPlanBadge(const QString& type, int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    if (type == "budget") {
        p.setBrush(QColor("#FEE2E2"));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(QRect(1, 1, size - 2, size - 2), 6, 6);
        p.setBrush(QColor("#E0173D"));
        p.drawRoundedRect(4, size - 9, 3, 5, 1, 1);
        p.drawRoundedRect(9, size - 15, 3, 11, 1, 1);
        p.drawRoundedRect(14, size - 12, 3, 8, 1, 1);
    } else {
        p.setBrush(QColor("#E0EDFF"));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(QRect(1, 1, size - 2, size - 2), 6, 6);
        p.setPen(QPen(QColor("#1E6FFF"), 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        p.drawLine(4, 7, 7, 7);
        p.drawLine(7, 7, 9, 15);
        p.drawLine(9, 15, 17, 15);
        p.drawLine(17, 15, 19, 9);
        p.drawLine(19, 9, 7, 9);
    }
    return pix;
}

QIcon createCategoryCircleIcon(const QString& categoryName, int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QString nameLower = categoryName.toLower();
    QColor bgColor("#F1F5F9");
    QColor borderColor("#E2E8F0");
    QString iconChar = QString::fromUtf8("🏷️");

    if (nameLower.contains("lương") || nameLower.contains("thưởng") || nameLower.contains("thu nhập")) {
        bgColor = QColor("#FEF3C7");
        borderColor = QColor("#FDE68A");
        iconChar = QString::fromUtf8("💰");
    } else if (nameLower.contains("ăn") || nameLower.contains("cơm") || nameLower.contains("cà phê")) {
        bgColor = QColor("#FEE2E2");
        borderColor = QColor("#FECACA");
        iconChar = QString::fromUtf8("🍜");
    } else if (nameLower.contains("iphone") || nameLower.contains("mua sắm") || nameLower.contains("quần áo")) {
        bgColor = QColor("#FFEDD5");
        borderColor = QColor("#FED7AA");
        iconChar = QString::fromUtf8("🛍️");
    } else if (nameLower.contains("trọ") || nameLower.contains("điện") || nameLower.contains("nhà")) {
        bgColor = QColor("#E0EDFF");
        borderColor = QColor("#BFDBFE");
        iconChar = QString::fromUtf8("🏠");
    } else if (nameLower.contains("đi lại") || nameLower.contains("xăng") || nameLower.contains("xe")) {
        bgColor = QColor("#CCFBF1");
        borderColor = QColor("#99F6E4");
        iconChar = QString::fromUtf8("🚗");
    }

    p.setBrush(bgColor);
    p.setPen(QPen(borderColor, 1));
    p.drawEllipse(1, 1, size - 2, size - 2);

    QFont font = p.font();
    font.setPixelSize(size * 0.5);
    p.setFont(font);
    p.drawText(QRect(0, 0, size, size), Qt::AlignCenter, iconChar);
    return QIcon(pix);
}

static QPixmap createHeaderPixmap() {
    QPixmap pix(18, 18);
    pix.fill(Qt::transparent);
    return pix;
}

QIcon createSortHeaderIcon() {
    QPixmap pix = createHeaderPixmap();
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#5B6B8C"), 1.8, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(5, 4, 5, 14);
    p.drawLine(3, 11, 5, 14);
    p.drawLine(7, 11, 5, 14);
    p.drawLine(13, 14, 13, 4);
    p.drawLine(11, 7, 13, 4);
    p.drawLine(15, 7, 13, 4);
    return QIcon(pix);
}

QIcon createCoinHeaderIcon() {
    QPixmap pix = createHeaderPixmap();
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#5B6B8C"), 1.6));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(3, 3, 12, 12);
    p.drawEllipse(6, 6, 6, 6);
    return QIcon(pix);
}

QIcon createCalendarHeaderIcon() {
    QPixmap pix = createHeaderPixmap();
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#5B6B8C"), 1.6));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(2, 4, 14, 12, 2, 2);
    p.drawLine(2, 8, 16, 8);
    p.drawLine(6, 2, 6, 5);
    p.drawLine(12, 2, 12, 5);
    return QIcon(pix);
}

QIcon createWalletHeaderIcon() {
    QPixmap pix = createHeaderPixmap();
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#5B6B8C"), 1.6));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(2, 4, 14, 10, 3, 3);
    p.drawPoint(12, 9);
    return QIcon(pix);
}

QIcon createTagHeaderIcon() {
    QPixmap pix = createHeaderPixmap();
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#5B6B8C"), 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPolygonF poly;
    poly << QPointF(3, 9) << QPointF(9, 3) << QPointF(15, 3) << QPointF(15, 9) << QPointF(9, 15);
    p.drawPolygon(poly);
    p.drawPoint(12, 6);
    return QIcon(pix);
}

QIcon createNoteHeaderIcon() {
    QPixmap pix = createHeaderPixmap();
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#5B6B8C"), 1.6));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(3, 2, 12, 14, 2, 2);
    p.drawLine(6, 6, 12, 6);
    p.drawLine(6, 9, 12, 9);
    p.drawLine(6, 12, 10, 12);
    return QIcon(pix);
}

} // namespace GuiIcons

