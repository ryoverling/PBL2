#pragma once

#include <QDialog>
#include <QDate>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QGridLayout>

class QDateEdit;

// Hộp thoại popup chọn ngày đầy đủ (Ngày - Tháng - Năm) và chọn Tháng/Năm
class DatePickerPopup : public QDialog {
    Q_OBJECT

public:
    enum class ViewMode {
        Days = 0,
        Months = 1,
        Years = 2
    };

    explicit DatePickerPopup(const QDate& initialDate = QDate::currentDate(), QWidget* parent = nullptr);
    ~DatePickerPopup() override = default;

    QDate getSelectedDate() const { return selectedDate; }
    void setSelectedDate(const QDate& date);

    void setMonthOnlyMode(bool enabled);

    // Gắn popup chọn ngày nâng cao vào bất kỳ QDateEdit nào
    static void attachToDateEdit(QDateEdit* dateEdit);

    // Mở popup chọn ngày tĩnh
    static bool selectDate(QWidget* parent, QDate& inOutDate, const QString& title = "Chọn ngày");

    // Mở popup chọn tháng & năm tĩnh (thay thế MonthYearPickerDialog cũ)
    static bool selectMonthYear(QWidget* parent, int& inOutMonth, int& inOutYear, const QString& title = "Chọn tháng và năm");

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onPrevClicked();
    void onNextClicked();
    void onTitleClicked();
    void onTodayClicked();
    void onSelectClicked();
    void onDayButtonClicked(int idx);
    void onMonthButtonClicked(int monthNum);
    void onYearButtonClicked(int year);

private:
    void setupUi();
    void setupHeaderRow(class QVBoxLayout* mainLayout);
    void setupFooterRow(class QVBoxLayout* mainLayout);
    void setupDaysView();
    void setupWeekdayHeader(class QVBoxLayout* layout);
    void setupMonthsView();
    void setupYearsView();
    void handleDaysKeyNavigation(int key, Qt::KeyboardModifiers modifiers);

    void updateHeader();
    void updateDaysGrid();
    void updateMonthsGrid();
    void updateYearsGrid();

    void setViewMode(ViewMode mode);
    static QIcon makeChevronIcon(bool left);
    static QIcon makeCalendarIcon();

    QDate selectedDate;
    int displayMonth;
    int displayYear;
    int decadeStartYear;
    ViewMode currentMode;
    bool monthOnlyMode = false;

    QPushButton* btnPrev = nullptr;
    QPushButton* btnTitle = nullptr;
    QPushButton* btnNext = nullptr;

    QStackedWidget* stackViews = nullptr;
    QWidget* pageDays = nullptr;
    QWidget* pageMonths = nullptr;
    QWidget* pageYears = nullptr;

    QGridLayout* daysGrid = nullptr;
    QPushButton* dayButtons[42]{};
    QDate dayButtonDates[42]{};

    QGridLayout* monthsGrid = nullptr;
    QPushButton* monthButtons[12]{};

    QGridLayout* yearsGrid = nullptr;
    QPushButton* yearButtons[12]{};

    QPushButton* btnToday = nullptr;
    QPushButton* btnCancel = nullptr;
    QPushButton* btnSelect = nullptr;
};
