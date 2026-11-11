#include "gui_icons.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QFont>

namespace GuiIcons {

QPixmap createExchangeBadge(int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    // Nền vuông bo tròn xanh pastel
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#E0EDFF"));
    p.drawRoundedRect(0, 0, size, size, 12, 12);

    // Vẽ hai mũi tên ngược chiều ⇄
    p.setPen(QPen(QColor("#1E6FFF"), 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    int cy = size / 2;
    int arrowW = size * 0.44;
    int x1 = (size - arrowW) / 2;
    int x2 = x1 + arrowW;

    // Mũi tên trên trỏ sang phải
    int yTop = cy - 4;
    p.drawLine(x1, yTop, x2, yTop);
    p.drawLine(x2 - 4, yTop - 4, x2, yTop);
    p.drawLine(x2 - 4, yTop + 4, x2, yTop);

    // Mũi tên dưới trỏ sang trái
    int yBot = cy + 5;
    p.drawLine(x2, yBot, x1, yBot);
    p.drawLine(x1 + 4, yBot - 4, x1, yBot);
    p.drawLine(x1 + 4, yBot + 4, x1, yBot);

    return pix;
}

QPixmap createResetIcon(int size, const QColor& color) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap));
    p.setBrush(Qt::NoBrush);

    QRectF arcRect(2, 2, size - 4, size - 4);
    p.drawArc(arcRect, 45 * 16, 270 * 16);

    // Vẽ đầu mũi tên
    p.drawLine(size - 3, size / 2 - 4, size - 3, size / 2);
    p.drawLine(size - 6, size / 2, size - 3, size / 2);

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

    // Vùng đổ bóng mờ gradient bên dưới
    QPainterPath fillPath = path;
    fillPath.lineTo(width - 4, height);
    fillPath.lineTo(4, height);
    fillPath.closeSubpath();

    p.fillPath(fillPath, fillColor);

    // Đường nét chính
    p.setPen(QPen(lineColor, 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPath(path);

    return pix;
}

QPixmap createMiniBars(int width, int height, const QColor& color) {
    QPixmap pix(width, height);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    // 4 cột tăng dần: 35%, 55%, 75%, 100%
    const qreal heights[4] = {0.35, 0.55, 0.75, 1.00};
    int barW = 8;
    int gap = 5;
    int totalW = 4 * barW + 3 * gap;
    int startX = width - totalW - 4;

    for (int i = 0; i < 4; ++i) {
        int bH = (int)(height * heights[i]);
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

    p.setPen(QPen(fg, 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    int c = size / 2;
    if (kind == "income") {
        p.drawLine(c - 6, c + 6, c + 6, c - 6);
        p.drawLine(c, c - 6, c + 6, c - 6);
        p.drawLine(c + 6, c, c + 6, c - 6);
    } else if (kind == "expense") {
        p.drawLine(c, c - 7, c, c + 7);
        p.drawLine(c - 5, c + 2, c, c + 7);
        p.drawLine(c + 5, c + 2, c, c + 7);
    } else {
        p.setPen(QPen(fg, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawLine(c - 7, c - 4, c, c - 8);
        p.drawLine(c, c - 8, c + 7, c - 4);
        p.drawLine(c + 7, c - 4, c, c);
        p.drawLine(c, c, c - 7, c - 4);

        p.drawLine(c - 7, c, c, c + 4);
        p.drawLine(c, c + 4, c + 7, c);

        p.drawLine(c - 7, c + 4, c, c + 8);
        p.drawLine(c, c + 8, c + 7, c + 4);
    }

    return pix;
}

static QIcon createSvgLikeHeaderIcon(const std::function<void(QPainter&)>& drawFunc) {
    QPixmap pix(18, 18);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    drawFunc(p);
    return QIcon(pix);
}

QIcon createSortHeaderIcon() {
    return createSvgLikeHeaderIcon([](QPainter& p) {
        p.setPen(QPen(QColor("#5B6B8C"), 1.8, Qt::SolidLine, Qt::RoundCap));
        // Mũi tên xuống bên trái
        p.drawLine(5, 4, 5, 14);
        p.drawLine(3, 11, 5, 14);
        p.drawLine(7, 11, 5, 14);
        // Mũi tên lên bên phải
        p.drawLine(13, 14, 13, 4);
        p.drawLine(11, 7, 13, 4);
        p.drawLine(15, 7, 13, 4);
    });
}

QIcon createCoinHeaderIcon() {
    return createSvgLikeHeaderIcon([](QPainter& p) {
        p.setPen(QPen(QColor("#5B6B8C"), 1.6));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(3, 3, 12, 12);
        p.drawEllipse(6, 6, 6, 6);
    });
}

QIcon createCalendarHeaderIcon() {
    return createSvgLikeHeaderIcon([](QPainter& p) {
        p.setPen(QPen(QColor("#5B6B8C"), 1.6));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(2, 4, 14, 12, 2, 2);
        p.drawLine(2, 8, 16, 8);
        p.drawLine(6, 2, 6, 5);
        p.drawLine(12, 2, 12, 5);
    });
}

QIcon createWalletHeaderIcon() {
    return createSvgLikeHeaderIcon([](QPainter& p) {
        p.setPen(QPen(QColor("#5B6B8C"), 1.6));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(2, 4, 14, 10, 3, 3);
        p.drawPoint(12, 9);
    });
}

QIcon createTagHeaderIcon() {
    return createSvgLikeHeaderIcon([](QPainter& p) {
        p.setPen(QPen(QColor("#5B6B8C"), 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QPolygonF poly;
        poly << QPointF(3, 9) << QPointF(9, 3) << QPointF(15, 3) << QPointF(15, 9) << QPointF(9, 15);
        p.drawPolygon(poly);
        p.drawPoint(12, 6);
    });
}

QIcon createNoteHeaderIcon() {
    return createSvgLikeHeaderIcon([](QPainter& p) {
        p.setPen(QPen(QColor("#5B6B8C"), 1.6));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(3, 2, 12, 14, 2, 2);
        p.drawLine(6, 6, 12, 6);
        p.drawLine(6, 9, 12, 9);
        p.drawLine(6, 12, 10, 12);
    });
}

QPixmap createWalletPageHeaderIcon(int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#E0EDFF"));
    p.drawRoundedRect(0, 0, size, size, 12, 12);

    p.setPen(QPen(QColor("#1E6FFF"), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(10, 13, size - 20, size - 26, 4, 4);
    p.drawLine(10, 19, size - 10, 19);
    p.setBrush(QColor("#1E6FFF"));
    p.drawEllipse(size - 18, size / 2 - 2, 4, 4);

    return pix;
}

QPixmap createHeroWalletIllustration(int width, int height) {
    QPixmap pix(width, height);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    // Vẽ ví màu xanh dương 3D
    QRect walletRect(25, 18, 85, 58);
    QLinearGradient wGrad(walletRect.topLeft(), walletRect.bottomRight());
    wGrad.setColorAt(0.0, QColor("#5B9DFF"));
    wGrad.setColorAt(1.0, QColor("#1E6FFF"));

    p.setPen(Qt::NoPen);
    p.setBrush(wGrad);
    p.drawRoundedRect(walletRect, 14, 14);

    // Nắp gập của ví
    QRect flapRect(25, 18, 85, 26);
    QLinearGradient fGrad(flapRect.topLeft(), flapRect.bottomLeft());
    fGrad.setColorAt(0.0, QColor("#7CB3FF"));
    fGrad.setColorAt(1.0, QColor("#3B82F6"));
    p.setBrush(fGrad);
    p.drawRoundedRect(flapRect, 14, 14);

    // Khóa ví tròn màu trắng
    p.setBrush(QColor("#FFFFFF"));
    p.drawRoundedRect(88, 38, 18, 14, 6, 6);
    p.setBrush(QColor("#1E6FFF"));
    p.drawEllipse(94, 42, 6, 6);

    // Chồng 2 đồng xu vàng phía trước bên trái
    auto drawCoin = [&](int cx, int cy, int cw, int ch) {
        QLinearGradient cGrad(cx, cy - ch/2, cx, cy + ch/2);
        cGrad.setColorAt(0.0, QColor("#FFE066"));
        cGrad.setColorAt(1.0, QColor("#F59E0B"));
        p.setBrush(cGrad);
        p.setPen(QPen(QColor("#D97706"), 1.2));
        p.drawEllipse(QPoint(cx, cy), cw, ch);
        p.setPen(QPen(QColor("#FBBF24"), 1.0));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPoint(cx, cy), cw - 3, ch - 2);
    };

    drawCoin(22, 66, 16, 8);
    drawCoin(38, 70, 18, 9);
    drawCoin(38, 64, 18, 9);

    return pix;
}

QPixmap createChevronCircle(int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(QPen(QColor("#E2E8F0"), 1.2));
    p.setBrush(QColor("#FFFFFF"));
    p.drawEllipse(1, 1, size - 2, size - 2);

    p.setPen(QPen(QColor("#5B6B8C"), 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    int c = size / 2;
    p.drawLine(c - 2, c - 4, c + 2, c);
    p.drawLine(c + 2, c, c - 2, c + 4);

    return pix;
}

QPixmap createWalletCardIcon(const QString& walletName, int walletType, int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QString lower = walletName.toLower();
    QColor bg("#EBF3FE");
    QColor fg("#1E6FFF");
    bool isPhone = false;

    if (lower.contains("momo") || walletType == 3) {
        bg = QColor("#F3EDFF");
        fg = QColor("#8B5CF6");
        isPhone = true;
    } else if (lower.contains("tiền mặt") || walletType == 1) {
        bg = QColor("#E4F8EE");
        fg = QColor("#0E8F4F");
    } else if (lower.contains("tp") || lower.contains("tpbank") || lower.contains("tp bank")) {
        bg = QColor("#FFF4E6");
        fg = QColor("#FF9F1A");
    }

    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    p.drawRoundedRect(0, 0, size, size, 12, 12);

    p.setPen(QPen(fg, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);

    if (isPhone) {
        // Icon điện thoại smartphone
        p.drawRoundedRect(size / 2 - 7, 8, 14, size - 16, 3, 3);
        p.drawLine(size / 2 - 3, size - 11, size / 2 + 3, size - 11);
    } else {
        // Icon ngân hàng: mái tam giác + 3 cột + đế
        p.drawLine(9, size - 11, size - 9, size - 11);
        p.drawLine(10, 15, size - 10, 15);
        p.drawLine(size / 2, 9, 9, 15);
        p.drawLine(size / 2, 9, size - 9, 15);
        p.drawLine(12, 15, 12, size - 11);
        p.drawLine(size / 2, 15, size / 2, size - 11);
        p.drawLine(size - 12, 15, size - 12, size - 11);
    }

    return pix;
}

QPixmap createWalletBadgeIcon(const QString& walletName, int walletType, int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QString lower = walletName.toLower();
    QColor bg("#EEF2F6");
    QColor fg("#4A5568");

    if (lower.contains("momo")) {
        bg = QColor("#FDF0F7");
        fg = QColor("#A50064");
    } else if (lower.contains("techcombank") || lower.contains("ngân hàng") || walletType == 1) {
        bg = QColor("#EAE8FE");
        fg = QColor("#5B47FB");
    } else if (lower.contains("tiền mặt") || walletType == 0) {
        bg = QColor("#E4F8EE");
        fg = QColor("#0E8F4F");
    }

    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    p.drawRoundedRect(0, 0, size, size, 6, 6);

    if (lower.contains("momo")) {
        QFont f = p.font();
        f.setPixelSize(8);
        f.setBold(true);
        p.setFont(f);
        p.setPen(fg);
        p.drawText(QRect(0, 0, size, size), Qt::AlignCenter, "mo\nmo");
    } else {
        p.setPen(QPen(fg, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        if (lower.contains("tiền mặt") || walletType == 0) {
            // Tờ tiền
            p.drawRoundedRect(4, 7, size - 8, size - 14, 2, 2);
            p.drawEllipse(size / 2 - 2, size / 2 - 2, 4, 4);
        } else {
            // Ngân hàng/cột nhà
            p.drawLine(4, size - 6, size - 4, size - 6);
            p.drawLine(5, 9, size - 5, 9);
            p.drawLine(size / 2, 5, 4, 9);
            p.drawLine(size / 2, 5, size - 4, 9);
            p.drawLine(7, 9, 7, size - 6);
            p.drawLine(size / 2, 9, size / 2, size - 6);
            p.drawLine(size - 7, 9, size - 7, size - 6);
        }
    }

    return pix;
}

} // namespace GuiIcons
