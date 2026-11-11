#pragma once

#include <QWidget>
#include "../../core/utils/utils.h"

// Lịch chi tiêu từng ngày trong tháng
class CalendarWidget : public QWidget {
    Q_OBJECT

public:
    explicit CalendarWidget(QWidget* parent = nullptr);
    void setData(int m, int y, const long long dailyExpense[31]);

signals:
    void dayClicked(int day);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    int month = 10;
    int year = 2026;
    long long expenses[31];
    bool hasData = false;
    int selectedDay = -1;
    int hoveredDay = -1;

    int getDayAtPos(const QPoint& pos) const;
    int gridRows() const;
    QRect cellRect(int row, int col) const;
    void drawHeaders(class QPainter& p);
    void drawCells(class QPainter& p);
    void drawCell(class QPainter& p, const QRect& r, int day, const Date& today);
    void drawExpenseBadge(class QPainter& p, const QRect& r, long long amount);
    void drawLegend(class QPainter& p);
};

