#include "calendar_widget.h"
#include "../../core/utils/utils.h"
#include <QPainter>
#include <QMouseEvent>
#include <QEvent>

CalendarWidget::CalendarWidget(QWidget* parent) : QWidget(parent) {
    for (int i = 0; i < 31; ++i) expenses[i] = 0;
    setMouseTracking(true);
}

void CalendarWidget::setData(int m, int y, const long long dailyExpense[31]) {
    if (month != m || year != y) selectedDay = -1;
    month = m;
    year = y;
    for (int i = 0; i < 31; ++i) expenses[i] = dailyExpense[i];
    hasData = true;
    update();
}

int CalendarWidget::gridRows() const {
    int occupied = Date::weekdayOfFirstDay(month, year) + Date::daysInMonth(month, year);
    return qMax(1, (occupied + 6) / 7);
}

QRect CalendarWidget::cellRect(int row, int col) const {
    const int gap = 4, top = 26, legendSpace = 32;
    int cellW = qMax(10, (width() - 12 - 6 * gap) / 7);
    int left = (width() - 7 * cellW - 6 * gap) / 2;
    int cellH = qMax(20, (height() - top - legendSpace - (gridRows() - 1) * gap) / gridRows());
    return QRect(left + col * (cellW + gap), top + row * (cellH + gap), cellW, cellH);
}

void CalendarWidget::drawHeaders(QPainter& p) {
    static const char* titles[] = {"T2", "T3", "T4", "T5", "T6", "T7", "CN"};
    QFont font = p.font(); font.setPixelSize(12); font.setBold(true); p.setFont(font);
    p.setPen(QColor("#64748B"));
    for (int col = 0; col < 7; ++col) {
        QRect r = cellRect(0, col);
        p.drawText(QRect(r.left(), 2, r.width(), 18), Qt::AlignCenter, titles[col]);
    }
}

void CalendarWidget::drawCell(QPainter& p, const QRect& r, int day, const Date& today) {
    long long amount = expenses[day - 1];
    bool isToday = day == today.day && month == today.month && year == today.year;
    QColor bg = amount == 0 ? QColor(isToday ? "#E8F2FF" : "#FFFFFF")
              : amount < 100000 ? QColor("#DDF7EF") : amount <= 500000 ? QColor("#FFF0D9") : QColor("#FFE5EA");
    p.setBrush(bg);
    QColor edge = selectedDay == day ? QColor("#0958D9") : isToday || hoveredDay == day ? QColor("#1677FF") : QColor("#E2ECFA");
    p.setPen(QPen(edge, selectedDay == day || isToday ? 2 : 1));
    p.drawRoundedRect(r.adjusted(1, 1, -1, -1), 10, 10);
    QFont font = p.font(); font.setPixelSize(12); font.setBold(amount > 0); p.setFont(font);
    p.setPen(QColor("#0B1F3A"));
    p.drawText(r.adjusted(8, 3, -4, -2), Qt::AlignLeft | Qt::AlignTop, QString::number(day));
    if (selectedDay == day) { p.setBrush(QColor("#0958D9")); p.setPen(Qt::NoPen); p.drawEllipse(r.right() - 10, r.top() + 6, 4, 4); }
    if (amount <= 0) return;
    font.setPixelSize(r.height() < 34 ? 12 : 14); font.setBold(true); p.setFont(font);
    p.setPen(QColor(amount < 100000 ? "#059669" : amount <= 500000 ? "#D97706" : "#E11D48"));
    p.drawText(QRect(r.left() + 2, r.bottom() - 19, r.width() - 4, 17), Qt::AlignCenter, QString::fromStdString(formatShortMoney(amount)));
}

void CalendarWidget::drawCells(QPainter& p) {
    int start = Date::weekdayOfFirstDay(month, year);
    Date today = Date::today();
    for (int day = 1; day <= Date::daysInMonth(month, year); ++day) {
        int slot = start + day - 1;
        drawCell(p, cellRect(slot / 7, slot % 7), day, today);
    }
}

void CalendarWidget::drawLegend(QPainter& p) {
    static const char* labels[] = {"Dưới 100k", "100k–500k", "Trên 500k"};
    static const char* colors[] = {"#DDF7EF", "#FFF0D9", "#FFE5EA"};
    QFont font = p.font(); font.setPixelSize(12); font.setBold(false); p.setFont(font);
    QFontMetrics fm(font);
    // Tính bề rộng chú giải để căn giữa dưới lịch.
    const int dotW = 11, dotTextGap = 6, groupGap = 24;
    int totalW = 0;
    int textWidths[3];
    for (int i = 0; i < 3; ++i) {
        textWidths[i] = fm.horizontalAdvance(QString::fromUtf8(labels[i]));
        totalW += dotW + dotTextGap + textWidths[i];
        if (i < 2) totalW += groupGap;
    }
    int y = cellRect(gridRows() - 1, 0).bottom() + 10;
    int startX = (width() - totalW) / 2;
    if (startX < 4) startX = 4;
    int x = startX;
    for (int i = 0; i < 3; ++i) {
        p.setBrush(QColor(colors[i])); p.setPen(Qt::NoPen);
        p.drawRoundedRect(QRect(x, y, dotW, dotW), 3, 3);
        p.setPen(QColor("#64748B"));
        p.drawText(QRect(x + dotW + dotTextGap, y - 2, textWidths[i] + 2, 16),
                   Qt::AlignLeft | Qt::AlignVCenter, QString::fromUtf8(labels[i]));
        x += dotW + dotTextGap + textWidths[i] + groupGap;
    }
}

int CalendarWidget::getDayAtPos(const QPoint& pos) const {
    int start = Date::weekdayOfFirstDay(month, year);
    for (int day = 1; day <= Date::daysInMonth(month, year); ++day) {
        int slot = start + day - 1;
        if (cellRect(slot / 7, slot % 7).contains(pos)) return day;
    }
    return -1;
}

void CalendarWidget::paintEvent(QPaintEvent*) {
    if (!hasData) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawHeaders(painter);
    drawCells(painter);
    drawLegend(painter);
}

void CalendarWidget::mousePressEvent(QMouseEvent* event) {
    int day = getDayAtPos(event->pos());
    if (day > 0) { selectedDay = day; update(); emit dayClicked(day); }
    QWidget::mousePressEvent(event);
}

void CalendarWidget::mouseMoveEvent(QMouseEvent* event) {
    int day = getDayAtPos(event->pos());
    if (day != hoveredDay) { hoveredDay = day; update(); }
    QWidget::mouseMoveEvent(event);
}

void CalendarWidget::leaveEvent(QEvent* event) {
    hoveredDay = -1;
    update();
    QWidget::leaveEvent(event);
}
