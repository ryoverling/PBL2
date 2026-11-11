#include "gui_icons_wallet.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>

namespace GuiIcons {

static void drawCoinHelper(QPainter& p, int cx, int cy, int cw, int ch) {
    QLinearGradient cGrad(cx, cy - ch / 2, cx, cy + ch / 2);
    cGrad.setColorAt(0.0, QColor("#FFE066"));
    cGrad.setColorAt(1.0, QColor("#F59E0B"));
    p.setBrush(cGrad);
    p.setPen(QPen(QColor("#D97706"), 1.2));
    p.drawEllipse(QPoint(cx, cy), cw, ch);
    p.setPen(QPen(QColor("#FBBF24"), 1.0));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(QPoint(cx, cy), cw - 3, ch - 2);
}

QPixmap createHeroWalletIllustration(int width, int height) {
    QPixmap pix(width, height);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QRect walletRect(25, 18, 85, 58);
    QLinearGradient wGrad(walletRect.topLeft(), walletRect.bottomRight());
    wGrad.setColorAt(0.0, QColor("#5B9DFF"));
    wGrad.setColorAt(1.0, QColor("#1E6FFF"));
    p.setPen(Qt::NoPen);
    p.setBrush(wGrad);
    p.drawRoundedRect(walletRect, 14, 14);

    QRect flapRect(25, 18, 85, 26);
    QLinearGradient fGrad(flapRect.topLeft(), flapRect.bottomLeft());
    fGrad.setColorAt(0.0, QColor("#7CB3FF"));
    fGrad.setColorAt(1.0, QColor("#3B82F6"));
    p.setBrush(fGrad);
    p.drawRoundedRect(flapRect, 14, 14);

    p.setBrush(QColor("#FFFFFF"));
    p.drawRoundedRect(88, 38, 18, 14, 6, 6);
    p.setBrush(QColor("#1E6FFF"));
    p.drawEllipse(94, 42, 6, 6);

    drawCoinHelper(p, 22, 66, 16, 8);
    drawCoinHelper(p, 38, 70, 18, 9);
    drawCoinHelper(p, 38, 64, 18, 9);

    return pix;
}

QPixmap createLoginIllustration(int width, int height) {
    QPixmap pix(width, height);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QLinearGradient grad(0, 0, width, height);
    grad.setColorAt(0.0, QColor("#1E6FFF"));
    grad.setColorAt(1.0, QColor("#0B2A6B"));
    p.setBrush(grad);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(width / 4, height / 3, width / 2, height / 2, 12, 12);

    p.setBrush(QColor("#3B82F6"));
    p.drawRoundedRect(width / 4 + 4, height / 3 + 4, width / 2 - 8, height / 5, 8, 8);

    p.setBrush(QColor("#F59E0B"));
    p.drawEllipse(width / 2 - 6, height / 2 - 2, 12, 12);

    p.setBrush(QColor("#F59E0B"));
    p.drawEllipse(width / 5, height / 2 + 10, 20, 20);
    p.setBrush(QColor("#FBBF24"));
    p.drawEllipse(width / 5 + 3, height / 2 + 13, 14, 14);

    return pix;
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
        bg = QColor("#F0E3FF");
        fg = QColor("#8E3FD8");
        isPhone = true;
    } else if (lower.contains("tiền mặt") || walletType == 1) {
        bg = QColor("#E4F8EE");
        fg = QColor("#0E8F4F");
    } else if (lower.contains("tp") || lower.contains("tpbank") || lower.contains("tp bank")) {
        bg = QColor("#FFF0D6");
        fg = QColor("#FF9F1A");
    }

    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    p.drawRoundedRect(0, 0, size, size, 12, 12);
    p.setPen(QPen(fg, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);

    if (isPhone) {
        p.drawRoundedRect(size / 2 - 7, 8, 14, size - 16, 3, 3);
        p.drawLine(size / 2 - 3, size - 11, size / 2 + 3, size - 11);
    } else {
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
        bg = QColor("#F0E3FF");
        fg = QColor("#8E3FD8");
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
            p.drawRoundedRect(4, 7, size - 8, size - 14, 2, 2);
            p.drawEllipse(size / 2 - 2, size / 2 - 2, 4, 4);
        } else {
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

QPixmap createStatisticsHeaderIcon(int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#E0EDFF"));
    p.drawRoundedRect(0, 0, size, size, 12, 12);

    p.setBrush(QColor("#1E6FFF"));
    p.drawRoundedRect(10, 20, 5, size - 30, 2, 2);
    p.drawRoundedRect(18, 14, 5, size - 24, 2, 2);
    p.drawRoundedRect(26, 24, 5, size - 34, 2, 2);
    return pix;
}

QPixmap createUserBadge(int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(QPen(QColor("#1E6FFF"), 1.8, Qt::SolidLine, Qt::RoundCap));
    p.setBrush(Qt::NoBrush);
    int c = size / 2;
    p.drawEllipse(c - 5, 4, 10, 10);
    p.drawArc(4, 15, size - 8, 16, 0, 180 * 16);
    return pix;
}

QPixmap createShieldBadge(int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(QPen(QColor("#1E6FFF"), 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    QPainterPath path;
    path.moveTo(size / 2, 4);
    path.lineTo(size - 6, 8);
    path.lineTo(size - 6, size / 2);
    path.quadTo(size - 6, size - 5, size / 2, size - 4);
    path.quadTo(6, size - 5, 6, size / 2);
    path.lineTo(6, 8);
    path.closeSubpath();
    p.drawPath(path);
    p.drawLine(size / 2 - 3, size / 2, size / 2 - 1, size / 2 + 3);
    p.drawLine(size / 2 - 1, size / 2 + 3, size / 2 + 4, size / 2 - 3);
    return pix;
}

QPixmap createLockBadge(int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(QPen(QColor("#1E6FFF"), 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(6, 12, size - 12, size - 16, 3, 3);
    p.drawArc(9, 5, size - 18, 14, 0, 180 * 16);
    return pix;
}

} // namespace GuiIcons

