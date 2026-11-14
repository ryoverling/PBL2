#include "month_picker_widget.h"
#include "date_picker_popup.h"
#include "../gui_helpers.h"
#include "../gui_icons_nav.h"
#include <QAction>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

namespace {
QIcon makeSmallChevron(bool left) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#1E293B"), 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if (left) {
        QPointF points[3] = { QPointF(12, 6), QPointF(8, 10), QPointF(12, 14) };
        p.drawPolyline(points, 3);
    } else {
        QPointF points[3] = { QPointF(8, 6), QPointF(12, 10), QPointF(8, 14) };
        p.drawPolyline(points, 3);
    }
    return QIcon(pix);
}
} // namespace

MonthPickerWidget::MonthPickerWidget(QWidget* parent)
    : MonthPickerWidget(QDate::currentDate().month(), QDate::currentDate().year(), parent) {}

MonthPickerWidget::MonthPickerWidget(int month, int year, QWidget* parent)
    : QWidget(parent), m_month(month), m_year(year) {
    if (m_month < 1 || m_month > 12) m_month = QDate::currentDate().month();
    if (m_year < 1900 || m_year > 2200) m_year = QDate::currentDate().year();
    setupUi();
    updateDisplayText();
}

void MonthPickerWidget::setupUi() {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    btnPrev = new QPushButton(this);
    btnPrev->setIcon(makeSmallChevron(true));
    btnPrev->setIconSize(QSize(16, 16));
    btnPrev->setFixedSize(36, 36);
    btnPrev->setCursor(Qt::PointingHandCursor);
    btnPrev->setToolTip("Tháng trước");

    edtDisplay = new QLineEdit(this);
    edtDisplay->setFixedHeight(36);
    edtDisplay->setMinimumWidth(150);
    edtDisplay->setMaximumWidth(165);
    edtDisplay->setReadOnly(true);
    edtDisplay->setCursor(Qt::PointingHandCursor);
    edtDisplay->setAlignment(Qt::AlignCenter);

    QAction* calAction = edtDisplay->addAction(GuiIcons::createCalendarIcon(QColor("#1E6FFF")), QLineEdit::TrailingPosition);
    calAction->setToolTip("Mở bộ chọn lịch tháng");
    connect(calAction, &QAction::triggered, this, &MonthPickerWidget::onFieldClicked);

    btnNext = new QPushButton(this);
    btnNext->setIcon(makeSmallChevron(false));
    btnNext->setIconSize(QSize(16, 16));
    btnNext->setFixedSize(36, 36);
    btnNext->setCursor(Qt::PointingHandCursor);
    btnNext->setToolTip("Tháng tiếp theo");

    layout->addWidget(btnPrev);
    layout->addWidget(edtDisplay);
    layout->addWidget(btnNext);

    connect(btnPrev, &QPushButton::clicked, this, &MonthPickerWidget::onPrevClicked);
    connect(btnNext, &QPushButton::clicked, this, &MonthPickerWidget::onNextClicked);

    edtDisplay->installEventFilter(this);
}

bool MonthPickerWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == edtDisplay && event->type() == QEvent::MouseButtonPress) {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            onFieldClicked();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void MonthPickerWidget::onPrevClicked() {
    m_month--;
    if (m_month < 1) {
        m_month = 12;
        m_year--;
    }
    updateDisplayText();
    emit monthYearChanged(m_month, m_year);
}

void MonthPickerWidget::onNextClicked() {
    m_month++;
    if (m_month > 12) {
        m_month = 1;
        m_year++;
    }
    updateDisplayText();
    emit monthYearChanged(m_month, m_year);
}

void MonthPickerWidget::onFieldClicked() {
    int m = m_month;
    int y = m_year;
    if (DatePickerPopup::selectMonthYear(window(), m, y, "Chọn tháng")) {
        if (m != m_month || y != m_year) {
            m_month = m;
            m_year = y;
            updateDisplayText();
            emit monthYearChanged(m_month, m_year);
        }
    }
}

void MonthPickerWidget::setMonthYear(int month, int year) {
    if (month >= 1 && month <= 12 && year >= 1900 && year <= 2200) {
        m_month = month;
        m_year = year;
        updateDisplayText();
    }
}

void MonthPickerWidget::updateDisplayText() {
    if (edtDisplay) {
        edtDisplay->setText(QString("Tháng %1/%2").arg(m_month).arg(m_year));
    }
}

