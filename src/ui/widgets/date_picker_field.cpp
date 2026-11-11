#include "date_picker_field.h"
#include "date_picker_popup.h"
#include "../gui_helpers.h"
#include <QMouseEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QApplication>

QIcon DatePickerField::createCalendarSvgIcon(const QColor& color) {
    return UiHelper::createCalendarIcon(color);
}

DatePickerField::DatePickerField(QWidget* parent)
    : QLineEdit(parent), m_date(QDate::currentDate()) {
    init();
    setDate(QDate::currentDate());
}

DatePickerField::DatePickerField(const QDate& initialDate, QWidget* parent)
    : QLineEdit(parent), m_date(initialDate.isValid() ? initialDate : QDate::currentDate()) {
    init();
    setDate(m_date);
}

void DatePickerField::init() {
    setPlaceholderText("dd/MM/yyyy");
    setMinimumHeight(40);
    setCursor(Qt::PointingHandCursor);

    m_calendarAction = addAction(createCalendarSvgIcon(QColor("#1A6BEB")), QLineEdit::TrailingPosition);
    m_calendarAction->setToolTip(QString::fromUtf8("Mở hộp thoại Chọn ngày"));
    connect(m_calendarAction, &QAction::triggered, this, &DatePickerField::openCalendarDialog);
    connect(this, &QLineEdit::textChanged, this, &DatePickerField::onUserTextChanged);

    updateAppearance();
}

void DatePickerField::setDate(const QDate& d) {
    m_date = d;
    m_hasError = false;
    setToolTip("");

    m_blockSignals = true;
    setText(m_date.isValid() ? m_date.toString("dd/MM/yyyy") : "");
    m_blockSignals = false;

    updateAppearance();
    emit dateChanged(m_date);
}

void DatePickerField::clearDate() {
    m_date = QDate();
    m_hasError = false;
    setToolTip("");

    m_blockSignals = true;
    clear();
    m_blockSignals = false;

    updateAppearance();
    emit dateChanged(m_date);
}

QString DatePickerField::toIsoString() const {
    return m_date.isValid() ? m_date.toString("yyyy-MM-dd") : "";
}

void DatePickerField::setIsoString(const QString& iso) {
    setDate(QDate::fromString(iso, "yyyy-MM-dd"));
}

void DatePickerField::openCalendarDialog() {
    if (!isEnabled() || isReadOnly()) return;

    QDate initial = m_date.isValid() ? m_date : QDate::currentDate();
    DatePickerPopup popup(initial, this);

    QPoint pos = mapToGlobal(QPoint(0, height() + 2));
    QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    if (pos.y() + popup.sizeHint().height() > screenRect.bottom()) {
        pos.setY(mapToGlobal(QPoint(0, 0)).y() - popup.sizeHint().height() - 2);
    }
    popup.move(pos);

    if (popup.exec() == QDialog::Accepted) {
        setDate(popup.getSelectedDate());
    }
}

void DatePickerField::mousePressEvent(QMouseEvent* event) {
    QLineEdit::mousePressEvent(event);
    if (!isEnabled() || isReadOnly()) return;
    openCalendarDialog();
}

void DatePickerField::focusOutEvent(QFocusEvent* event) {
    QLineEdit::focusOutEvent(event);
    QString t = text().trimmed();
    if (t.isEmpty()) {
        m_hasError = false;
        m_date = QDate();
        setToolTip("");
    } else {
        QDate d = QDate::fromString(t, "dd/MM/yyyy");
        if (d.isValid()) {
            m_hasError = false;
            m_date = d;
            setToolTip("");
            m_blockSignals = true;
            setText(m_date.toString("dd/MM/yyyy"));
            m_blockSignals = false;
            emit dateChanged(m_date);
        } else {
            m_hasError = true;
            setToolTip(QString::fromUtf8("Ngày không hợp lệ (định dạng dd/MM/yyyy)"));
        }
    }
    updateAppearance();
}

void DatePickerField::handleDigitInput(QKeyEvent* event) {
    int pos = cursorPosition();
    QString curText = text();

    // Tự động chèn dấu '/' khi người dùng gõ ngày hoặc tháng
    if ((pos == 2 && curText.length() >= 2 && curText[1] != '/') ||
        (pos == 5 && curText.length() >= 5 && curText[4] != '/')) {
        insert("/");
    }

    QLineEdit::keyPressEvent(event);

    QString after = text();
    if (after.length() == 2 && !after.contains('/')) {
        setText(after + "/");
        setCursorPosition(3);
    } else if (after.length() == 5 && after.count('/') == 1) {
        setText(after + "/");
        setCursorPosition(6);
    }
}

void DatePickerField::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete ||
        event->key() == Qt::Key_Left || event->key() == Qt::Key_Right ||
        event->key() == Qt::Key_Tab || event->key() == Qt::Key_Backtab) {
        QLineEdit::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        openCalendarDialog();
        return;
    }

    QString inputChar = event->text();
    if (!inputChar.isEmpty() && inputChar[0].isDigit()) {
        handleDigitInput(event);
        return;
    }

    if (inputChar == "/") {
        QLineEdit::keyPressEvent(event);
        return;
    }

    event->ignore();
}

void DatePickerField::changeEvent(QEvent* event) {
    QLineEdit::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        updateAppearance();
    }
}

void DatePickerField::onUserTextChanged(const QString& text) {
    if (m_blockSignals) return;

    QString clean = text.trimmed();
    if (clean.isEmpty()) {
        m_hasError = false;
        m_date = QDate();
        setToolTip("");
        updateAppearance();
        emit dateChanged(m_date);
        return;
    }

    if (clean.length() == 10) {
        QDate d = QDate::fromString(clean, "dd/MM/yyyy");
        if (d.isValid()) {
            m_hasError = false;
            m_date = d;
            setToolTip("");
            updateAppearance();
            emit dateChanged(m_date);
        } else {
            m_hasError = true;
            setToolTip(QString::fromUtf8("Ngày không hợp lệ (không tồn tại trong lịch)"));
            updateAppearance();
        }
    } else {
        m_hasError = false;
        updateAppearance();
    }
}

void DatePickerField::updateAppearance() {
    if (!isEnabled()) {
        setStyleSheet(
            "DatePickerField { background-color: #F8FAFD; color: #94A3B8; border: 1px solid #CBD5E1; "
            "border-radius: 9px; min-height: 40px; padding: 0 34px 0 12px; font-size: 13px; font-weight: 500; }"
        );
        updateIcon();
        return;
    }

    if (m_hasError) {
        setStyleSheet(
            "DatePickerField { background-color: #FFF5F5; color: #EF4444; border: 1.5px solid #EF4444; "
            "border-radius: 9px; min-height: 40px; padding: 0 34px 0 12px; font-size: 13px; font-weight: 600; } "
            "DatePickerField:focus { border: 1.5px solid #DC2626; }"
        );
        updateIcon();
        return;
    }

    setStyleSheet(
        "DatePickerField { background-color: #FFFFFF; color: #102748; border: 1px solid #D7E4F4; "
        "border-radius: 9px; min-height: 40px; padding: 0 34px 0 12px; font-size: 13px; font-weight: 500; } "
        "DatePickerField:focus { border: 1.5px solid #1A6BEB; }"
    );
    updateIcon();
}

void DatePickerField::updateIcon() {
    if (!m_calendarAction) return;
    if (!isEnabled()) {
        m_calendarAction->setIcon(createCalendarSvgIcon(QColor("#CBD5E1")));
    } else if (m_hasError) {
        m_calendarAction->setIcon(createCalendarSvgIcon(QColor("#EF4444")));
    } else {
        m_calendarAction->setIcon(createCalendarSvgIcon(QColor("#1A6BEB")));
    }
}

