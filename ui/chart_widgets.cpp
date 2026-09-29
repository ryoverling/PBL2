#include "chart_widgets.h"

#include <QPainter>
#include <QPaintEvent>
#include <QtMath>
#include <algorithm>

DonutChartWidget::DonutChartWidget(QWidget* parent)
    : QWidget(parent) {
    setMinimumHeight(250);
}

QSize DonutChartWidget::minimumSizeHint() const {
    return QSize(360, 250);
}

void DonutChartWidget::setData(const QVector<ChartSlice>& data) {
    slices = data;
    update();
}

void DonutChartWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF area = rect().adjusted(16, 16, -16, -16);
    double legendWidth = qMin(180.0, area.width() * 0.42);
    QRectF chartArea(area.left(), area.top(), area.width() - legendWidth - 12, area.height());

    double diameter = qMin(chartArea.width(), chartArea.height()) * 0.82;
    QRectF pieRect(
        chartArea.center().x() - diameter / 2,
        chartArea.center().y() - diameter / 2,
        diameter,
        diameter
    );

    double total = 0;
    for (const auto& slice : slices) {
        if (slice.value > 0) total += slice.value;
    }

    if (total <= 0) {
        painter.setPen(QColor("#8b8e96"));
        painter.drawText(area, Qt::AlignCenter, "Chưa có dữ liệu chi tiêu");
        return;
    }

    int startAngle = 90 * 16;
    for (const auto& slice : slices) {
        if (slice.value <= 0) continue;
        int span = -qRound((slice.value / total) * 360.0 * 16.0);
        painter.setPen(Qt::NoPen);
        painter.setBrush(slice.color);
        painter.drawPie(pieRect, startAngle, span);
        startAngle += span;
    }

    double hole = diameter * 0.54;
    QRectF holeRect(
        pieRect.center().x() - hole / 2,
        pieRect.center().y() - hole / 2,
        hole,
        hole
    );
    painter.setBrush(QColor("#ffffff"));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(holeRect);

    painter.setPen(QColor("#202124"));
    QFont totalFont = painter.font();
    totalFont.setPointSize(12);
    totalFont.setBold(true);
    painter.setFont(totalFont);
    painter.drawText(holeRect, Qt::AlignCenter, "Chi tiêu");

    double x = area.right() - legendWidth;
    double y = area.top() + 8;
    QFont legendFont = painter.font();
    legendFont.setPointSize(9);
    legendFont.setBold(false);
    painter.setFont(legendFont);

    int shown = 0;
    for (const auto& slice : slices) {
        if (slice.value <= 0 || shown >= 7) continue;

        painter.setBrush(slice.color);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(QRectF(x, y + 4, 10, 10), 2, 2);

        painter.setPen(QColor("#33353a"));
        QString text = QString("%1  %2%")
            .arg(slice.label)
            .arg(QString::number(slice.value * 100.0 / total, 'f', 0));
        painter.drawText(QRectF(x + 18, y, legendWidth - 18, 20),
                         Qt::AlignLeft | Qt::AlignVCenter, text);

        y += 28;
        shown++;
    }
}

BarChartWidget::BarChartWidget(QWidget* parent)
    : QWidget(parent) {
    setMinimumHeight(250);
}

QSize BarChartWidget::minimumSizeHint() const {
    return QSize(420, 250);
}

void BarChartWidget::setData(const QStringList& newLabels, const QVector<double>& newValues) {
    labels = newLabels;
    values = newValues;
    update();
}

void BarChartWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF area = rect().adjusted(24, 20, -18, -36);
    if (values.isEmpty()) {
        painter.setPen(QColor("#8b8e96"));
        painter.drawText(area, Qt::AlignCenter, "Chưa có dữ liệu");
        return;
    }

    double maxValue = 0;
    for (double value : values) maxValue = qMax(maxValue, value);
    if (maxValue <= 0) maxValue = 1;

    painter.setPen(QPen(QColor("#e6e8ed"), 1));
    for (int i = 0; i <= 4; ++i) {
        double y = area.bottom() - area.height() * i / 4.0;
        painter.drawLine(QPointF(area.left(), y), QPointF(area.right(), y));
    }

    int n = values.size();
    double slot = area.width() / qMax(1, n);
    double barWidth = qMin(44.0, slot * 0.56);

    for (int i = 0; i < n; ++i) {
        double ratio = qBound(0.0, values[i] / maxValue, 1.0);
        double h = area.height() * ratio;
        double x = area.left() + slot * i + (slot - barWidth) / 2;
        QRectF bar(x, area.bottom() - h, barWidth, h);

        painter.setBrush(QColor("#6c4ed9"));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(bar, 6, 6);

        painter.setPen(QColor("#6f727a"));
        QString label = i < labels.size() ? labels[i] : QString::number(i + 1);
        painter.drawText(
            QRectF(area.left() + slot * i, area.bottom() + 8, slot, 22),
            Qt::AlignHCenter | Qt::AlignTop,
            label
        );
    }
}
