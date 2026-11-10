#pragma once

#include <QWidget>
#include "../../core/utils/utils.h"

class QPainter;

// Biểu đồ chỉ vẽ dữ liệu do MoneyManager cung cấp.
class ChartWidget : public QWidget {
    Q_OBJECT
private:
    MonthStat stats[6];
    bool hasData = false;
    long long findMaxAmount() const;
    void drawLegend(QPainter& painter);
    void drawAxes(QPainter& painter, const QRect& plot, long long maximum);
    void drawTick(QPainter& painter, const QRect& plot, int y, long long value);
    void drawColumns(QPainter& painter, const QRect& plot, long long maximum);
    void drawBar(QPainter& painter, const QRect& plot, int x, int width,
                 long long amount, long long maximum, const QColor& color);
protected:
    void paintEvent(QPaintEvent* event) override;
public:
    explicit ChartWidget(QWidget* parent = nullptr);
    void setData(const MonthStat inStats[6]);
};
