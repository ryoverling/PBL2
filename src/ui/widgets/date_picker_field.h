#pragma once

#include <QLineEdit>
#include <QDate>
#include <QAction>

// Component chuẩn dùng chung: ô nhập ngày kèm icon lịch và dialog chọn ngày
class DatePickerField : public QLineEdit {
    Q_OBJECT
    Q_PROPERTY(QDate date READ date WRITE setDate USER true)

public:
    explicit DatePickerField(QWidget* parent = nullptr);
    explicit DatePickerField(const QDate& initialDate, QWidget* parent = nullptr);
    ~DatePickerField() override = default;

    // Lấy ngày hiện tại trong ô (trả về QDate không hợp lệ nếu ô trống hoặc sai)
    QDate date() const { return m_date; }
    void setDate(const QDate& d);

    // Kiểm tra ngày nhập có tồn tại thực tế và hợp lệ
    bool isValidDate() const { return m_date.isValid(); }
    void clearDate();

    // Định dạng chuỗi yyyy-MM-dd phục vụ file_storage
    QString toIsoString() const;
    void setIsoString(const QString& iso);

public slots:
    void openCalendarDialog();

signals:
    void dateChanged(const QDate& date);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onUserTextChanged(const QString& text);

private:
    void init();
    void updateAppearance();
    void updateIcon();
    void handleDigitInput(QKeyEvent* event);
    static QIcon createCalendarSvgIcon(const QColor& color);

    QDate m_date;
    QAction* m_calendarAction = nullptr;
    bool m_blockSignals = false;
    bool m_hasError = false;
};

