#include "donut_widget.h"
#include "../gui_helpers.h"
#include <QPainter>
#include <cmath>

static const double kPi = 3.14159265358979323846;

DonutWidget::DonutWidget(QWidget* parent) : QWidget(parent) { setMinimumHeight(200); }

void DonutWidget::setData(const LinkedList<CategoryExpense>& categories, long long totalExpense) {
    static const char* colors[6] = {"#1E6FFF", "#0E8F4F", "#E0173D", "#FF9F1A", "#8B5CF6", "#5B6B8C"};
    count = 0;
    total = totalExpense;
    for (const Node<CategoryExpense>* cur = categories.getHead(); cur; cur = cur->next)
        appendCategory(cur->data);
    for (int i = 0; i < count; ++i) slices[i].color = QColor(colors[i]);
    setToolTip(UiHelper::formatMoneyVND(total));
    update();
}

void DonutWidget::appendCategory(const CategoryExpense& item) {
    if (count < 5) {
        Slice& slice = slices[count++];
        slice.name = QString::fromStdString(item.categoryName);
        slice.amount = item.totalExpense;
        slice.percentage = item.percentage;
    } else {
        if (count == 5) { slices[5] = Slice(); slices[5].name = "Khác"; count = 6; }
        slices[5].amount += item.totalExpense;
        slices[5].percentage += item.percentage;
    }
}

void DonutWidget::drawDonut(QPainter& painter) {
    int diameter = qMin(216, qMin(width(), height()) - 16);
    QRectF circle((width() - diameter) / 2.0, (height() - diameter) / 2.0, diameter, diameter);
    int start = 90 * 16;
    // Lưu góc bắt đầu và độ rộng của từng phần để đặt nhãn.
    int starts[6], sweeps[6];
    for (int i = 0; i < count; ++i) {
        int sweep = i == count - 1 ? -5760 + (90 * 16 - start)
                    : -static_cast<int>(std::round(slices[i].amount * 5760.0 / total));
        starts[i] = start;
        sweeps[i] = sweep;
        painter.setBrush(slices[i].color);
        painter.drawPie(circle, start, sweep);
        start += sweep;
    }
    // Khoét tâm trắng và ghi tổng chi.
    painter.setBrush(QColor("#FFFFFF"));
    painter.setPen(QPen(QColor("#DCE8FA"), 1));
    int inset = diameter * 0.23;
    painter.drawEllipse(circle.adjusted(inset, inset, -inset, -inset));

    // Vẽ biểu tượng ví tiền nhỏ ở tâm
    int cx = static_cast<int>(circle.center().x());
    int cy = static_cast<int>(circle.center().y()) - 26;
    painter.setPen(QPen(QColor("#1E6FFF"), 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(QRectF(cx - 10, cy - 7, 20, 14), 3, 3);
    painter.drawLine(cx + 2, cy - 1, cx + 5, cy - 1);

    painter.setPen(QColor("#5B6B8C"));
    painter.setFont(QFont("Segoe UI", 9));
    painter.drawText(QRectF(circle.left(), circle.center().y() - 10, diameter, 20), Qt::AlignCenter, "Tổng chi");

    painter.setPen(QColor("#0B2A6B"));
    QFont amountFont = painter.font();
    amountFont.setBold(true);
    amountFont.setPointSize(11);
    painter.setFont(amountFont);
    QRectF amountRect(circle.left() + inset / 2, circle.center().y() + 10, diameter - inset, 26);
    QString amount = painter.fontMetrics().elidedText(UiHelper::formatMoneyVND(total), Qt::ElideRight, amountRect.width());
    painter.drawText(amountRect, Qt::AlignCenter, amount);
    drawPercentageLabels(painter, circle, diameter, starts, sweeps);
}

void DonutWidget::drawPercentageLabels(QPainter& painter, const QRectF& circle, int diameter,
                                        const int starts[], const int sweeps[]) {
    double outerR = diameter / 2.0;
    double innerR = outerR * 0.54;
    double midR = (outerR + innerR) / 2.0;
    QPointF center = circle.center();

    QFont labelFont("Segoe UI", 9);
    labelFont.setBold(true);
    painter.setFont(labelFont);
    QFontMetrics fm(labelFont);

    for (int i = 0; i < count; ++i) {
        int pct = slices[i].percentage;
        if (pct < 6) continue; // Phần quá nhỏ sẽ không đủ chỗ ghi nhãn.

        // Qt lưu góc theo đơn vị 1/16 độ, chiều dương ngược kim đồng hồ.
        double startDeg = starts[i] / 16.0;
        double sweepDeg = sweeps[i] / 16.0;
        double midDeg = startDeg + sweepDeg / 2.0;
        double radMid = midDeg * kPi / 180.0;

        double lx = center.x() + midR * std::cos(radMid);
        double ly = center.y() - midR * std::sin(radMid);

        QString pctStr = QString::number(pct) + "%";
        int tw = fm.horizontalAdvance(pctStr);
        int th = fm.height();

        QRectF labelRect(lx - tw / 2.0 - 2, ly - th / 2.0, tw + 4, th);

        // Chọn màu chữ đủ tương phản với nền phần biểu đồ.
        QColor bg = slices[i].color;
        int lum = bg.red() * 299 + bg.green() * 587 + bg.blue() * 114;
        QColor textColor = (lum / 1000 > 140) ? QColor("#102748") : QColor("#FFFFFF");

        painter.setPen(textColor);
        painter.drawText(labelRect, Qt::AlignCenter, pctStr);
    }
}

void DonutWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setFont(QFont("Segoe UI", 11));
    if (total <= 0 || count == 0) {
        painter.setPen(QColor("#536986"));
        painter.drawText(rect(), Qt::AlignCenter, "Chưa có khoản chi");
        return;
    }
    drawDonut(painter);
}
