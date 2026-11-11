#include "date_picker_popup.h"
#include "../gui_helpers.h"
#include <QDateEdit>
#include <QLineEdit>
#include <QAction>
#include <QKeyEvent>
#include <QPainter>
#include <QPixmap>
#include <QEvent>
#include <algorithm>

// Biểu tượng chevron vector cho 2 nút điều hướng
QIcon DatePickerPopup::makeChevronIcon(bool left) {
    QPixmap pix(24, 24);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#1E293B"), 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if (left) {
        QPointF points[3] = { QPointF(14, 7), QPointF(9, 12), QPointF(14, 17) };
        p.drawPolyline(points, 3);
    } else {
        QPointF points[3] = { QPointF(10, 7), QPointF(15, 12), QPointF(10, 17) };
        p.drawPolyline(points, 3);
    }
    return QIcon(pix);
}

// Biểu tượng lịch chuyên dụng nằm bên phải trong ô
QIcon DatePickerPopup::makeCalendarIcon() {
    return UiHelper::createCalendarIcon(QColor("#1A6BEB"));
}

namespace {

class DateEditFilter : public QObject {
public:
    explicit DateEditFilter(QDateEdit* edit) : QObject(edit), targetEdit(edit) {}

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        if (event->type() == QEvent::MouseButtonPress) {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                QDate curDate = targetEdit->date();
                DatePickerPopup popup(curDate, targetEdit);
                QPoint pos = targetEdit->mapToGlobal(QPoint(0, targetEdit->height() + 2));
                popup.move(pos);
                if (popup.exec() == QDialog::Accepted) {
                    targetEdit->setDate(popup.getSelectedDate());
                }
                return true;
            }
        }
        return QObject::eventFilter(watched, event);
    }

private:
    QDateEdit* targetEdit;
};
} // namespace

void DatePickerPopup::attachToDateEdit(QDateEdit* dateEdit) {
    if (!dateEdit) return;

    dateEdit->setCalendarPopup(false);
    dateEdit->setDisplayFormat("dd/MM/yyyy");
    dateEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);

    QLineEdit* le = dateEdit->findChild<QLineEdit*>();
    if (le) {
        le->setReadOnly(false);
        le->setCursor(Qt::PointingHandCursor);

        QAction* calAction = le->addAction(makeCalendarIcon(), QLineEdit::TrailingPosition);
        calAction->setToolTip("Mở bộ chọn lịch Ngày - Tháng - Năm");
        QObject::connect(calAction, &QAction::triggered, [dateEdit]() {
            QDate curDate = dateEdit->date();
            DatePickerPopup popup(curDate, dateEdit);
            QPoint pos = dateEdit->mapToGlobal(QPoint(0, dateEdit->height() + 2));
            popup.move(pos);
            if (popup.exec() == QDialog::Accepted) {
                dateEdit->setDate(popup.getSelectedDate());
            }
        });

        auto* filter = new DateEditFilter(dateEdit);
        le->installEventFilter(filter);
    }
}

bool DatePickerPopup::selectDate(QWidget* parent, QDate& inOutDate, const QString& title) {
    DatePickerPopup popup(inOutDate.isValid() ? inOutDate : QDate::currentDate(), parent);
    popup.setWindowTitle(title);
    if (popup.exec() == QDialog::Accepted) {
        inOutDate = popup.getSelectedDate();
        return true;
    }
    return false;
}

bool DatePickerPopup::selectMonthYear(QWidget* parent, int& inOutMonth, int& inOutYear, const QString& title) {
    if (inOutYear < 1900 || inOutYear > 2200) inOutYear = QDate::currentDate().year();
    if (inOutMonth < 1 || inOutMonth > 12) inOutMonth = QDate::currentDate().month();

    DatePickerPopup popup(QDate(inOutYear, inOutMonth, 1), parent);
    popup.setWindowTitle(title.isEmpty() ? "Chọn ngày" : title);
    // Luôn hiển thị bảng lịch có chọn ngày theo yêu cầu người dùng
    if (popup.exec() == QDialog::Accepted) {
        inOutMonth = popup.selectedDate.month();
        inOutYear = popup.selectedDate.year();
        return true;
    }
    return false;
}

DatePickerPopup::DatePickerPopup(const QDate& initialDate, QWidget* parent)
    : QDialog(parent),
      selectedDate(initialDate.isValid() ? initialDate : QDate::currentDate()),
      displayMonth(selectedDate.month()),
      displayYear(selectedDate.year()),
      decadeStartYear((selectedDate.year() / 12) * 12),
      currentMode(ViewMode::Days),
      monthOnlyMode(false) {

    setWindowTitle("Chọn ngày");
    setMinimumWidth(360);
    resize(380, 480);
    UiHelper::setupWindowTheme(this);

    setupUi();
    setViewMode(ViewMode::Days);
}

void DatePickerPopup::setMonthOnlyMode(bool enabled) {
    monthOnlyMode = enabled;
    if (monthOnlyMode) {
        setWindowTitle("Chọn tháng và năm");
        btnToday->setText("Tháng này");
        setViewMode(ViewMode::Months);
    } else {
        setWindowTitle("Chọn ngày");
        btnToday->setText("Hôm nay");
        setViewMode(ViewMode::Days);
    }
}

void DatePickerPopup::setSelectedDate(const QDate& date) {
    if (!date.isValid()) return;
    selectedDate = date;
    displayMonth = date.month();
    displayYear = date.year();
    decadeStartYear = (date.year() / 12) * 12;
    updateDaysGrid();
    updateMonthsGrid();
    updateHeader();
}

void DatePickerPopup::setupHeaderRow(QVBoxLayout* mainLayout) {
    auto* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(8);

    btnPrev = new QPushButton(this);
    btnPrev->setObjectName("btnPrevMonth");
    btnPrev->setIcon(makeChevronIcon(true));
    btnPrev->setIconSize(QSize(18, 18));
    btnPrev->setFixedSize(38, 38);
    btnPrev->setCursor(Qt::PointingHandCursor);

    btnTitle = new QPushButton(this);
    btnTitle->setObjectName("btnTitle");
    btnTitle->setMinimumHeight(38);
    btnTitle->setCursor(Qt::PointingHandCursor);
    btnTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    btnNext = new QPushButton(this);
    btnNext->setObjectName("btnNextMonth");
    btnNext->setIcon(makeChevronIcon(false));
    btnNext->setIconSize(QSize(18, 18));
    btnNext->setFixedSize(38, 38);
    btnNext->setCursor(Qt::PointingHandCursor);

    headerLayout->addWidget(btnPrev);
    headerLayout->addWidget(btnTitle);
    headerLayout->addWidget(btnNext);
    mainLayout->addLayout(headerLayout);

    connect(btnPrev, &QPushButton::clicked, this, &DatePickerPopup::onPrevClicked);
    connect(btnNext, &QPushButton::clicked, this, &DatePickerPopup::onNextClicked);
    connect(btnTitle, &QPushButton::clicked, this, &DatePickerPopup::onTitleClicked);
}

void DatePickerPopup::setupFooterRow(QVBoxLayout* mainLayout) {
    auto* footerLayout = new QHBoxLayout();
    footerLayout->setSpacing(12);

    btnToday = new QPushButton("Hôm nay", this);
    btnToday->setObjectName("btnToday");
    btnToday->setFixedHeight(40);
    btnToday->setCursor(Qt::PointingHandCursor);

    btnCancel = new QPushButton("Đóng", this);
    btnCancel->setObjectName("btnPopupCancel");
    btnCancel->setFixedHeight(40);
    btnCancel->setMinimumWidth(82);
    btnCancel->setCursor(Qt::PointingHandCursor);

    btnSelect = new QPushButton("Chọn", this);
    btnSelect->setObjectName("btnPopupSelect");
    btnSelect->setFixedHeight(40);
    btnSelect->setMinimumWidth(88);
    btnSelect->setCursor(Qt::PointingHandCursor);

    footerLayout->addWidget(btnToday);
    footerLayout->addStretch(1);
    footerLayout->addWidget(btnCancel);
    footerLayout->addWidget(btnSelect);
    mainLayout->addLayout(footerLayout);

    connect(btnToday, &QPushButton::clicked, this, &DatePickerPopup::onTodayClicked);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnSelect, &QPushButton::clicked, this, &DatePickerPopup::onSelectClicked);
}

void DatePickerPopup::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 16, 20, 18);
    mainLayout->setSpacing(10);

    setupHeaderRow(mainLayout);

    // Stacked widget cho 3 chế độ xem (Ngày, Tháng, Năm)
    stackViews = new QStackedWidget(this);
    setupDaysView();
    setupMonthsView();
    setupYearsView();

    stackViews->addWidget(pageDays);
    stackViews->addWidget(pageMonths);
    stackViews->addWidget(pageYears);
    mainLayout->addWidget(stackViews, 1);

    setupFooterRow(mainLayout);
}

void DatePickerPopup::setupWeekdayHeader(QVBoxLayout* layout) {
    auto* daysHeaderLayout = new QHBoxLayout();
    daysHeaderLayout->setSpacing(4);
    static const char* weekdays[7] = {"T2", "T3", "T4", "T5", "T6", "T7", "CN"};
    for (int i = 0; i < 7; ++i) {
        QLabel* lbl = new QLabel(QString::fromUtf8(weekdays[i]), pageDays);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setFont(QFont("Segoe UI", 10, QFont::DemiBold));
        lbl->setStyleSheet(i == 6 ? "color: #E0173D; font-weight: 700; font-size: 13px;" : "color: #5B6B8C; font-weight: 600; font-size: 13px;");
        daysHeaderLayout->addWidget(lbl, 1);
    }
    layout->addLayout(daysHeaderLayout);
}

void DatePickerPopup::onDayButtonClicked(int idx) {
    selectedDate = dayButtonDates[idx];
    displayMonth = selectedDate.month();
    displayYear = selectedDate.year();
    updateDaysGrid();
    updateHeader();
}

void DatePickerPopup::onMonthButtonClicked(int monthNum) {
    displayMonth = monthNum;
    int daysInM = QDate(displayYear, displayMonth, 1).daysInMonth();
    int day = qBound(1, selectedDate.day(), daysInM);
    selectedDate = QDate(displayYear, displayMonth, day);
    if (monthOnlyMode) {
        updateMonthsGrid();
    } else {
        setViewMode(ViewMode::Days);
    }
}

void DatePickerPopup::onYearButtonClicked(int year) {
    displayYear = year;
    setViewMode(ViewMode::Months);
}

void DatePickerPopup::setupDaysView() {
    pageDays = new QWidget(this);
    auto* layout = new QVBoxLayout(pageDays);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    setupWeekdayHeader(layout);

    // Lưới 6 hàng x 7 cột các ngày (spacing 4px)
    daysGrid = new QGridLayout();
    daysGrid->setSpacing(4);
    for (int r = 0; r < 6; ++r) {
        daysGrid->setRowStretch(r, 1);
        for (int c = 0; c < 7; ++c) {
            if (r == 0) daysGrid->setColumnStretch(c, 1);
            int idx = r * 7 + c;
            auto* btn = new QPushButton(pageDays);
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            btn->setMinimumSize(40, 40);
            btn->setCursor(Qt::PointingHandCursor);
            connect(btn, &QPushButton::clicked, this, [this, idx]() { onDayButtonClicked(idx); });
            daysGrid->addWidget(btn, r, c);
            dayButtons[idx] = btn;
        }
    }
    layout->addLayout(daysGrid);
}

void DatePickerPopup::setupMonthsView() {
    pageMonths = new QWidget(this);
    monthsGrid = new QGridLayout(pageMonths);
    monthsGrid->setContentsMargins(0, 8, 0, 8);
    monthsGrid->setSpacing(10);

    for (int c = 0; c < 3; ++c) monthsGrid->setColumnStretch(c, 1);
    for (int r = 0; r < 4; ++r) monthsGrid->setRowStretch(r, 1);

    static const char* mNames[12] = {
        "Tháng 1", "Tháng 2", "Tháng 3", "Tháng 4",
        "Tháng 5", "Tháng 6", "Tháng 7", "Tháng 8",
        "Tháng 9", "Tháng 10", "Tháng 11", "Tháng 12"
    };

    for (int i = 0; i < 12; ++i) {
        auto* btn = new QPushButton(QString::fromUtf8(mNames[i]), pageMonths);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        btn->setMinimumHeight(44);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFont(QFont("Segoe UI", 10, QFont::DemiBold));
        int monthNum = i + 1;
        connect(btn, &QPushButton::clicked, this, [this, monthNum]() { onMonthButtonClicked(monthNum); });
        monthsGrid->addWidget(btn, i / 3, i % 3);
        monthButtons[i] = btn;
    }
}

void DatePickerPopup::setupYearsView() {
    pageYears = new QWidget(this);
    yearsGrid = new QGridLayout(pageYears);
    yearsGrid->setContentsMargins(0, 8, 0, 8);
    yearsGrid->setSpacing(10);

    for (int c = 0; c < 3; ++c) monthsGrid->setColumnStretch(c, 1);
    for (int r = 0; r < 4; ++r) monthsGrid->setRowStretch(r, 1);

    for (int i = 0; i < 12; ++i) {
        auto* btn = new QPushButton(pageYears);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        btn->setMinimumHeight(44);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFont(QFont("Segoe UI", 10, QFont::DemiBold));
        int targetYear = decadeStartYear + i;
        connect(btn, &QPushButton::clicked, this, [this, targetYear]() { onYearButtonClicked(targetYear); });
        yearsGrid->addWidget(btn, i / 3, i % 3);
        yearButtons[i] = btn;
    }
}

void DatePickerPopup::setViewMode(ViewMode mode) {
    currentMode = mode;
    stackViews->setCurrentIndex(static_cast<int>(mode));
    updateHeader();

    if (mode == ViewMode::Days) {
        updateDaysGrid();
    } else if (mode == ViewMode::Months) {
        updateMonthsGrid();
    } else if (mode == ViewMode::Years) {
        updateYearsGrid();
    }
}

void DatePickerPopup::updateHeader() {
    if (currentMode == ViewMode::Days) {
        btnTitle->setText(QString("Tháng %1, %2").arg(displayMonth).arg(displayYear));
    } else if (currentMode == ViewMode::Months) {
        btnTitle->setText(QString("Năm %1  ▾").arg(displayYear));
    } else if (currentMode == ViewMode::Years) {
        btnTitle->setText(QString("%1 – %2").arg(decadeStartYear).arg(decadeStartYear + 11));
    }
}

void DatePickerPopup::updateDaysGrid() {
    QDate firstOfMonth(displayYear, displayMonth, 1);
    int firstCol = firstOfMonth.dayOfWeek() - 1;
    QDate startDate = firstOfMonth.addDays(-firstCol);
    QDate today = QDate::currentDate();

    for (int i = 0; i < 42; ++i) {
        QDate d = startDate.addDays(i);
        dayButtonDates[i] = d;
        QPushButton* btn = dayButtons[i];
        btn->setText(QString::number(d.day()));

        bool isCurrentMonth = (d.month() == displayMonth);
        bool isSelected = (d == selectedDate);
        bool isToday = (d == today);
        bool isSunday = (d.dayOfWeek() == 7);

        QString style;
        if (isSelected) {
            style = "QPushButton { background-color: #1E6FFF; color: #FFFFFF; font-weight: 700; border-radius: 10px; border: none; font-size: 14px; padding: 0; }";
        } else if (!isCurrentMonth) {
            // Ngày tháng khác màu xám nhạt (#A0AEC0)
            style = "QPushButton { background-color: transparent; color: #A0AEC0; border: none; border-radius: 10px; font-size: 14px; padding: 0; } QPushButton:hover { background-color: #EBF3FE; color: #1E6FFF; }";
        } else if (isSunday) {
            // Chủ nhật màu đỏ (#E0173D)
            style = "QPushButton { background-color: transparent; color: #E0173D; font-weight: 600; border: none; border-radius: 10px; font-size: 14px; padding: 0; } QPushButton:hover { background-color: #EBF3FE; color: #1E6FFF; }";
        } else if (isToday) {
            style = "QPushButton { background-color: #EBF3FE; color: #1E6FFF; font-weight: 700; border-radius: 10px; border: 1.5px solid #1E6FFF; font-size: 14px; padding: 0; } QPushButton:hover { background-color: #DCE8FA; }";
        } else {
            style = "QPushButton { background-color: transparent; color: #0B2A6B; font-weight: 500; border: none; border-radius: 10px; font-size: 14px; padding: 0; } QPushButton:hover { background-color: #EBF3FE; color: #1E6FFF; }";
        }
        btn->setStyleSheet(style);
    }
}

void DatePickerPopup::updateMonthsGrid() {
    int curMonth = QDate::currentDate().month();
    int curYear = QDate::currentDate().year();
    for (int i = 0; i < 12; ++i) {
        QPushButton* btn = monthButtons[i];
        bool isSelected = (displayYear == selectedDate.year() && (i + 1) == selectedDate.month());
        bool isCurrent = (displayYear == curYear && (i + 1) == curMonth);
        if (isSelected) {
            btn->setStyleSheet("QPushButton { background-color: #1A6BEB; color: #FFFFFF; font-weight: bold; border-radius: 10px; border: none; font-size: 13px; padding: 0 4px; }");
        } else if (isCurrent) {
            btn->setStyleSheet("QPushButton { background-color: #F0F6FF; color: #1A6BEB; font-weight: bold; border: 1.5px solid #1A6BEB; border-radius: 10px; font-size: 13px; padding: 0 4px; } QPushButton:hover { background-color: #EBF3FE; }");
        } else {
            btn->setStyleSheet("QPushButton { background-color: #FFFFFF; color: #102748; font-weight: 600; border: 1px solid #D7E4F4; border-radius: 10px; font-size: 13px; padding: 0 4px; } QPushButton:hover { background-color: #E8F1FF; color: #1A6BEB; border-color: #1A6BEB; }");
        }
    }
}

void DatePickerPopup::updateYearsGrid() {
    int curYear = QDate::currentDate().year();
    for (int i = 0; i < 12; ++i) {
        int yr = decadeStartYear + i;
        QPushButton* btn = yearButtons[i];
        btn->setText(QString::number(yr));
        bool isSelected = (yr == selectedDate.year());
        bool isCurrent = (yr == curYear);
        if (isSelected) {
            btn->setStyleSheet("QPushButton { background-color: #1A6BEB; color: #FFFFFF; font-weight: bold; border-radius: 10px; border: none; font-size: 13px; padding: 0; }");
        } else if (isCurrent) {
            btn->setStyleSheet("QPushButton { background-color: #F0F6FF; color: #1A6BEB; font-weight: bold; border: 1.5px solid #1A6BEB; border-radius: 10px; font-size: 13px; padding: 0; } QPushButton:hover { background-color: #EBF3FE; }");
        } else {
            btn->setStyleSheet("QPushButton { background-color: #FFFFFF; color: #102748; font-weight: 600; border: 1px solid #D7E4F4; border-radius: 10px; font-size: 13px; padding: 0; } QPushButton:hover { background-color: #E8F1FF; color: #1A6BEB; border-color: #1A6BEB; }");
        }
    }
}

void DatePickerPopup::onPrevClicked() {
    if (currentMode == ViewMode::Days) {
        displayMonth--;
        if (displayMonth < 1) { displayMonth = 12; displayYear--; }
        updateDaysGrid();
    } else if (currentMode == ViewMode::Months) {
        displayYear--;
        updateMonthsGrid();
    } else if (currentMode == ViewMode::Years) {
        decadeStartYear -= 12;
        updateYearsGrid();
    }
    updateHeader();
}

void DatePickerPopup::onNextClicked() {
    if (currentMode == ViewMode::Days) {
        displayMonth++;
        if (displayMonth > 12) { displayMonth = 1; displayYear++; }
        updateDaysGrid();
    } else if (currentMode == ViewMode::Months) {
        displayYear++;
        updateMonthsGrid();
    } else if (currentMode == ViewMode::Years) {
        decadeStartYear += 12;
        updateYearsGrid();
    }
    updateHeader();
}

void DatePickerPopup::onTitleClicked() {
    if (currentMode == ViewMode::Days) {
        setViewMode(ViewMode::Months);
    } else if (currentMode == ViewMode::Months) {
        setViewMode(ViewMode::Years);
    } else {
        if (monthOnlyMode) {
            setViewMode(ViewMode::Months);
        } else {
            setViewMode(ViewMode::Days);
        }
    }
}

void DatePickerPopup::onTodayClicked() {
    QDate today = QDate::currentDate();
    selectedDate = today;
    displayMonth = today.month();
    displayYear = today.year();
    decadeStartYear = (today.year() / 12) * 12;

    if (monthOnlyMode) {
        updateMonthsGrid();
        updateHeader();
    } else {
        setViewMode(ViewMode::Days);
    }
}

void DatePickerPopup::onSelectClicked() {
    accept();
}

void DatePickerPopup::handleDaysKeyNavigation(int key, Qt::KeyboardModifiers modifiers) {
    QDate newDate = selectedDate;
    switch (key) {
        case Qt::Key_Left:
            newDate = newDate.addDays(-1);
            break;
        case Qt::Key_Right:
            newDate = newDate.addDays(1);
            break;
        case Qt::Key_Up:
            newDate = newDate.addDays(-7);
            break;
        case Qt::Key_Down:
            newDate = newDate.addDays(7);
            break;
        case Qt::Key_PageUp:
            newDate = (modifiers & Qt::ControlModifier) ? newDate.addYears(-1) : newDate.addMonths(-1);
            break;
        case Qt::Key_PageDown:
            newDate = (modifiers & Qt::ControlModifier) ? newDate.addYears(1) : newDate.addMonths(1);
            break;
        default:
            return;
    }
    if (newDate.isValid()) {
        setSelectedDate(newDate);
    }
}

void DatePickerPopup::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        reject();
        return;
    }
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        accept();
        return;
    }

    if (currentMode == ViewMode::Days) {
        handleDaysKeyNavigation(event->key(), event->modifiers());
    } else {
        QDialog::keyPressEvent(event);
    }
}
