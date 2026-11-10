#include "icon_badge.h"
#include <QPainter>

IconBadge::IconBadge(QWidget* parent) : QWidget(parent) {
    setMinimumSize(36, 36);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

static void drawArrow(QPainter& p, bool up) {
    if (up) {
        // Mui ten cheo len phai ↗
        p.drawLine(13, 30, 29, 14);
        p.drawLine(20, 14, 29, 14);
        p.drawLine(29, 14, 29, 23);
    } else {
        // Mui ten thang xuong duoi ↓
        p.drawLine(22, 12, 22, 31);
        p.drawLine(15, 24, 22, 31);
        p.drawLine(29, 24, 22, 31);
    }
}

static void drawCash(QPainter& p) {
    p.drawRoundedRect(QRectF(9, 14, 26, 17), 3, 3);
    p.drawEllipse(QPointF(22, 22.5), 4, 4);
    p.drawLine(12, 18, 14, 18);
    p.drawLine(30, 27, 32, 27);
}

static void drawBank(QPainter& p) {
    p.drawLine(9, 18, 22, 10); p.drawLine(22, 10, 35, 18);
    p.drawLine(10, 20, 34, 20);
    for (int x : {13, 21, 29}) p.drawLine(x, 22, x, 31);
    p.drawLine(9, 34, 35, 34);
}

static void drawElectronic(QPainter& p) {
    p.drawRoundedRect(QRectF(15, 8, 16, 28), 3, 3);
    p.drawLine(18, 13, 28, 13);
    p.drawEllipse(QPointF(23, 30), 1, 1);
    p.drawArc(QRectF(5, 18, 13, 13), -60 * 16, 120 * 16);
}

static void drawBalance(QPainter& p) {
    // 2 dong xu xep chong
    p.drawEllipse(QRectF(10, 14, 24, 10));
    p.drawLine(10, 19, 10, 26);
    p.drawLine(34, 19, 34, 26);
    p.drawArc(QRectF(10, 21, 24, 10), 180 * 16, 180 * 16);
}

static void drawScale(QPainter& p) {
    // Can tieu ly
    p.drawLine(22, 10, 22, 33);
    p.drawLine(15, 33, 29, 33);
    p.drawLine(11, 15, 33, 15);
    p.drawLine(11, 15, 7, 24);
    p.drawLine(11, 15, 15, 24);
    p.drawLine(6, 24, 16, 24);
    p.drawLine(33, 15, 29, 24);
    p.drawLine(33, 15, 37, 24);
    p.drawLine(28, 24, 38, 24);
}

void IconBadge::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.scale(width() / 44.0, height() / 44.0);
    QString kind = property("kind").toString();
    bool income = kind == "income", expense = kind == "expense";

    QColor bg = (kind == "cash" || income) ? QColor("#E6FAF3")
              : expense ? QColor("#FEECEB")
              : (kind == "ewallet") ? QColor("#F3EDFF")
              : QColor("#E0EDFF");

    QColor stroke = (kind == "cash" || income) ? QColor("#0E8F4F")
                  : expense ? QColor("#E0173D")
                  : (kind == "ewallet") ? QColor("#8B5CF6")
                  : QColor("#1E6FFF");

    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    p.drawRoundedRect(QRectF(1, 1, 42, 42), 12, 12);

    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(stroke, 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if (kind == "cash") drawCash(p);
    else if (kind == "bank") drawBank(p);
    else if (kind == "ewallet") drawElectronic(p);
    else if (kind == "balance") drawBalance(p);
    else if (kind == "net") drawScale(p);
    else if (income || expense) drawArrow(p, income);
}
