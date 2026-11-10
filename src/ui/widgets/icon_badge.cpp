#include "icon_badge.h"
#include <QPainter>

IconBadge::IconBadge(QWidget* parent) : QWidget(parent) {
    setMinimumSize(36, 36);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

static void drawArrow(QPainter& p, bool up) {
    int tip = up ? 12 : 32, tail = up ? 32 : 12;
    p.drawLine(22, tail, 22, tip);
    p.drawLine(22, tip, 15, up ? tip + 7 : tip - 7);
    p.drawLine(22, tip, 29, up ? tip + 7 : tip - 7);
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

void IconBadge::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.scale(width() / 44.0, height() / 44.0);
    QString kind = property("kind").toString();
    bool income = kind == "income", expense = kind == "expense";
    QColor bg = kind == "cash" ? QColor("#DDF7EF") : kind == "bank" ? QColor("#E8F2FF")
              : kind == "ewallet" ? QColor("#F3EDFF") : income ? QColor("#E7FAF3")
              : expense ? QColor("#FFF0F3") : QColor("#E8F2FF");
    QColor stroke = kind == "cash" ? QColor("#059669") : kind == "ewallet" ? QColor("#8B5CF6")
                  : income ? QColor("#10B981") : expense ? QColor("#FF405C") : QColor("#1677FF");
    p.setPen(Qt::NoPen); p.setBrush(bg); p.drawRoundedRect(QRectF(1, 1, 42, 42), 10, 10);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(stroke, 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if (kind == "cash") drawCash(p);
    else if (kind == "bank") drawBank(p);
    else if (kind == "ewallet") drawElectronic(p);
    else if (income || expense) drawArrow(p, income);
}
