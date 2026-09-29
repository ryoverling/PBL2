#pragma once

#include <QDialog>

class QComboBox;
class QDoubleSpinBox;
class QDateEdit;
class QWidget;

class Budget;
class CategoryManager;

class BudgetDialog : public QDialog {
private:
    CategoryManager* categories;
    const Budget* existing;

    QDoubleSpinBox* amountInput;
    QComboBox* categoryCombo;
    QComboBox* periodCombo;
    QDateEdit* referenceDate;
    QDateEdit* customStart;
    QDateEdit* customEnd;
    QWidget* referenceRow;
    QWidget* customRow;

    void populateCategories();
    void updatePeriodUi();
    void validateAndAccept();

public:
    BudgetDialog(CategoryManager* categories,
                 const Budget* existing = nullptr,
                 QWidget* parent = nullptr);

    double amount() const;
    int categoryId() const;
    QString startDate() const;
    QString endDate() const;
};
