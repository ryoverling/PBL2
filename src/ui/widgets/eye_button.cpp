#include "eye_button.h"
#include <QPainter>

EyeButton::EyeButton(QWidget *parent) : QPushButton(parent) {
    setCheckable(true);
    setCursor(Qt::PointingHandCursor);
    setFixedSize(28, 28);
}

static void drawEye(QPainter& painter, qreal cx, qreal cy, qreal rx, qreal ry,
                    const QColor& color, bool crossed) {
    painter.setPen(QPen(color, 1.8));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPointF(cx, cy), rx, ry);
    painter.setBrush(color);
    painter.drawEllipse(QPointF(cx, cy), 3.0, 3.0);
    if (crossed) {
        painter.setPen(QPen(color, 2.0));
        painter.drawLine(QPointF(cx - rx, cy + ry), QPointF(cx + rx, cy - ry));
    }
}

void EyeButton::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    qreal cx = width() / 2.0;
    qreal cy = height() / 2.0;
    qreal rx = width() * 0.38;
    qreal ry = height() * 0.24;

    QColor color = QColor("#64748B");
    if (isDown() || underMouse()) {
        color = QColor("#1677FF");
    }
    drawEye(painter, cx, cy, rx, ry, color, isChecked());
}

