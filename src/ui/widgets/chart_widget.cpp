#include "chart_widget.h"
#include "../gui_helpers.h"
#include <QPainter>
#include <algorithm>

ChartWidget::ChartWidget(QWidget* parent) : QWidget(parent) {}

void ChartWidget::setData(const MonthStat inStats[6]) {
    for (int i = 0; i < 6; ++i) stats[i] = inStats[i];
    hasData = true;
    update();
}

long long ChartWidget::findMaxAmount() const {
    long long maximum = 1;
    for (int i = 0; i < 6; ++i) {
        maximum = std::max(maximum, stats[i].totalIncome);
        maximum = std::max(maximum, stats[i].totalExpense);
    }
    return maximum;
}

void ChartWidget::drawBar(QPainter& painter, const QRect& plot, int x, int width,
                          long long amount, long long maximum, const QColor& color) {
    // Số tiền bằng 0 không vẽ cột; không nâng chiều cao giả.
    if (amount <= 0) return;
    double height = static_cast<double>(amount) / maximum * plot.height();
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRoundedRect(QRectF(x, plot.bottom() - height, width, height), 4, 4);
}

void ChartWidget::drawColumns(QPainter& painter, const QRect& plot, long long maximum) {
    int groupWidth = plot.width() / 6;
    int barWidth = std::clamp((groupWidth - 12) / 2, 1, 32);
    for (int i = 0; i < 6; ++i) {
        int left = plot.left() + i * groupWidth;
        int x = left + (groupWidth - barWidth * 2 - 3) / 2;
        drawBar(painter, plot, x, barWidth, stats[i].totalIncome, maximum, QColor("#12B886"));
        drawBar(painter, plot, x + barWidth + 3, barWidth, stats[i].totalExpense, maximum, QColor("#EF4765"));
        painter.setPen(QColor("#536986"));
        QString label = QString("%1/%2").arg(stats[i].month).arg(stats[i].year % 100, 2, 10, QChar('0'));
        painter.drawText(QRect(left, plot.bottom() + 6, groupWidth, 24), Qt::AlignCenter, label);
    }
}

void ChartWidget::paintEvent(QPaintEvent*) {
    if (!hasData) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(QFont("Segoe UI", 9));
    long long maximum = findMaxAmount();
    int left = painter.fontMetrics().horizontalAdvance(UiHelper::formatMoneyVND(maximum)) + 14;
    QRect plot(left, 40, width() - left - 12, height() - 76);
    if (plot.width() < 60 || plot.height() < 20) return;
    drawLegend(painter);
    drawAxes(painter, plot, maximum);
    drawColumns(painter, plot, maximum);
}
