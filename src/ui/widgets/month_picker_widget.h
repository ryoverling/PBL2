#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDate>

// Component bộ chọn tháng gọn: [ < ] [ Tháng MM/YYYY 📅 ] [ > ]
class MonthPickerWidget : public QWidget {
    Q_OBJECT

public:
    explicit MonthPickerWidget(QWidget* parent = nullptr);
    explicit MonthPickerWidget(int month, int year, QWidget* parent = nullptr);
    ~MonthPickerWidget() override = default;

    int month() const { return m_month; }
    int year() const { return m_year; }

    void setMonthYear(int month, int year);

signals:
    void monthYearChanged(int month, int year);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onPrevClicked();
    void onNextClicked();
    void onFieldClicked();

private:
    void setupUi();
    void updateDisplayText();

    int m_month;
    int m_year;

    QPushButton* btnPrev = nullptr;
    QLineEdit* edtDisplay = nullptr;
    QPushButton* btnNext = nullptr;
};

