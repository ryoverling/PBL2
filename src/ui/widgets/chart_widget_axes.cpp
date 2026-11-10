#include "chart_widget.h"
#include "../gui_helpers.h"
#include <QPainter>
#include <algorithm>

void ChartWidget::drawLegend(QPainter& painter) {
    painter.setPen(QColor("#536986"));
    painter.drawText(4, 19, "Đơn vị: đồng");

    // Income legend
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#12B886"));
    painter.drawRoundedRect(QRect(width() - 170, 8, 12, 12), 3, 3);
    painter.setPen(QColor("#536986"));
    painter.drawText(width() - 152, 19, "Thu");

    // Expense legend
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#EF4765"));
    painter.drawRoundedRect(QRect(width() - 90, 8, 12, 12), 3, 3);
    painter.setPen(QColor("#536986"));
    painter.drawText(width() - 72, 19, "Chi");
}

void ChartWidget::drawTick(QPainter& painter, const QRect& plot, int y, long long value) {
    painter.setPen(QPen(QColor("#EDF2F8"), 1, Qt::DashLine));
    painter.drawLine(plot.left(), y, plot.right(), y);
    painter.setPen(QColor("#BACFE8"));
    painter.drawLine(plot.left() - 4, y, plot.left(), y);
    painter.setPen(QColor("#536986"));
    painter.drawText(QRect(0, y - 10, plot.left() - 8, 20),
                     Qt::AlignRight | Qt::AlignVCenter, UiHelper::formatMoneyVND(value));
}

void ChartWidget::drawAxes(QPainter& painter, const QRect& plot, long long maximum) {
    int ticks = static_cast<int>(std::min(4LL, maximum));
    for (int i = 0; i <= ticks; ++i) {
        int y = plot.bottom() - plot.height() * i / ticks;
        long long value = maximum / ticks * i + maximum % ticks * i / ticks;
        drawTick(painter, plot, y, value);
    }
    painter.setPen(QColor("#D7E4F4"));
    painter.drawLine(plot.bottomLeft(), plot.topLeft());
    painter.drawLine(plot.bottomLeft(), plot.bottomRight());
}
