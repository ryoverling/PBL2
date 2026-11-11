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
    const int gap = 6, top = 26;
    int cellW = qMax(10, (width() - 12 - 6 * gap) / 7);
    int left = (width() - 7 * cellW - 6 * gap) / 2;
    int cellH = qMax(20, (height() - top - (gridRows() - 1) * gap) / gridRows());
    return QRect(left + col * (cellW + gap), top + row * (cellH + gap), cellW, cellH);
}

void CalendarWidget::drawHeaders(QPainter& p) {
    static const char* titles[] = {"T2", "T3", "T4", "T5", "T6", "T7", "CN"};
    QFont font = p.font(); font.setPixelSize(12); font.setBold(true); p.setFont(font);
    p.setPen(QColor("#5B6B8C"));
    for (int col = 0; col < 7; ++col) {
        QRect r = cellRect(0, col);
        p.drawText(QRect(r.left(), 2, r.width(), 18), Qt::AlignCenter, titles[col]);
    }
}

void CalendarWidget::drawExpenseBadge(QPainter& p, const QRect& r, long long amount) {
    QColor badgeBg = (amount < 100000) ? QColor("#E6FAF3") : (amount <= 500000) ? QColor("#FFF4E5") : QColor("#FEECEB");
    QColor badgeText = (amount < 100000) ? QColor("#0E8F4F") : (amount <= 500000) ? QColor("#FF9F1A") : QColor("#E0173D");

    QString shortText = QString::fromStdString(formatShortMoney(amount));
    QFont font = p.font();
    font.setPixelSize(11);
    font.setBold(true);
    p.setFont(font);

    QFontMetrics fm(font);
    int textW = fm.horizontalAdvance(shortText);
    int badgeW = textW + 8;
    int badgeH = 17;
    int badgeX = r.right() - badgeW - 6;
    int badgeY = r.top() + 4;

    QRect badgeRect(badgeX, badgeY, badgeW, badgeH);
    p.setBrush(badgeBg);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(badgeRect, 4, 4);

    p.setPen(badgeText);
    p.drawText(badgeRect, Qt::AlignCenter, shortText);
}

void CalendarWidget::drawCell(QPainter& p, const QRect& r, int day, const Date& today) {
    long long amount = expenses[day - 1];
    bool isToday = (day == today.day && month == today.month && year == today.year);
    bool isSelected = (selectedDay == day);

    // Nền ô màu trắng, viền #DCE8FA. Nếu được chọn thì viền xanh đậm 2px
    QColor bg = isSelected ? QColor("#F4F8FF") : isToday ? QColor("#EAF2FF") : QColor("#FFFFFF");
    p.setBrush(bg);
    QColor edge = (isSelected || isToday) ? QColor("#1E6FFF") : (hoveredDay == day) ? QColor("#93C5FD") : QColor("#DCE8FA");
    p.setPen(QPen(edge, (isSelected || isToday) ? 2 : 1));
    p.drawRoundedRect(r.adjusted(1, 1, -1, -1), 10, 10);

    // Số ngày ở góc trên bên trái
    QFont font = p.font();
    font.setPixelSize(12);
    font.setBold(isSelected || isToday || amount > 0);
    p.setFont(font);
    p.setPen(QColor("#0B2A6B"));
    p.drawText(r.adjusted(8, 4, -4, -2), Qt::AlignLeft | Qt::AlignTop, QString::number(day));

    if (amount > 0) {
        drawExpenseBadge(p, r, amount);
    }
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
    static const char* bgColors[] = {"#E6FAF3", "#FFF4E5", "#FEECEB"};
    static const char* borderColors[] = {"#B7EBCE", "#FCD34D", "#FFCCD5"};

    QFont font = p.font(); font.setPixelSize(12); font.setBold(false); p.setFont(font);
    QFontMetrics fm(font);
    const int dotW = 12, dotTextGap = 6, groupGap = 20;
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
        p.setBrush(QColor(bgColors[i]));
        p.setPen(QPen(QColor(borderColors[i]), 1));
        p.drawRoundedRect(QRect(x, y, dotW, dotW), 3, 3);
        p.setPen(QColor("#5B6B8C"));
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
