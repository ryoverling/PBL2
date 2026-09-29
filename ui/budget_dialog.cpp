#include "budget_dialog.h"

#include "../manager/category_manager.h"
#include "../model/budget.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>

BudgetDialog::BudgetDialog(CategoryManager* categories,
                           const Budget* existing,
                           QWidget* parent)
    : QDialog(parent),
      categories(categories),
      existing(existing) {

    setWindowTitle(existing ? "Sửa ngân sách" : "Tạo ngân sách");
    setMinimumWidth(480);

    amountInput = new QDoubleSpinBox();
    amountInput->setRange(1, 1000000000000.0);
    amountInput->setDecimals(0);
    amountInput->setSingleStep(100000);
    amountInput->setSuffix(" đ");

    categoryCombo = new QComboBox();
    populateCategories();

    periodCombo = new QComboBox();
    periodCombo->addItem("Theo tuần", 0);
    periodCombo->addItem("Theo tháng", 1);
    periodCombo->addItem("Theo năm", 2);
    periodCombo->addItem("Tùy chọn", 3);

    referenceDate = new QDateEdit(QDate::currentDate());
    referenceDate->setCalendarPopup(true);
    referenceDate->setDisplayFormat("dd/MM/yyyy");

    customStart = new QDateEdit(QDate::currentDate());
    customStart->setCalendarPopup(true);
    customStart->setDisplayFormat("dd/MM/yyyy");

    customEnd = new QDateEdit(QDate::currentDate().addDays(6));
    customEnd->setCalendarPopup(true);
    customEnd->setDisplayFormat("dd/MM/yyyy");

    referenceRow = new QWidget();
    QHBoxLayout* referenceLayout = new QHBoxLayout(referenceRow);
    referenceLayout->setContentsMargins(0, 0, 0, 0);
    referenceLayout->addWidget(referenceDate);

    customRow = new QWidget();
    QHBoxLayout* customLayout = new QHBoxLayout(customRow);
    customLayout->setContentsMargins(0, 0, 0, 0);
    customLayout->addWidget(customStart);
    customLayout->addWidget(new QLabel("đến"));
    customLayout->addWidget(customEnd);

    if (existing) {
        amountInput->setValue(existing->getAmount());

        int categoryIndex = categoryCombo->findData(existing->getCategoryId());
        if (categoryIndex >= 0) categoryCombo->setCurrentIndex(categoryIndex);

        periodCombo->setCurrentIndex(3);
        customStart->setDate(QDate::fromString(
            QString::fromStdString(existing->getStartDate()), "yyyy-MM-dd"
        ));
        customEnd->setDate(QDate::fromString(
            QString::fromStdString(existing->getEndDate()), "yyyy-MM-dd"
        ));
    }

    QFormLayout* form = new QFormLayout();
    form->addRow("Số tiền", amountInput);
    form->addRow("Danh mục", categoryCombo);
    form->addRow("Chu kỳ", periodCombo);
    form->addRow("Mốc thời gian", referenceRow);
    form->addRow("Khoảng tùy chọn", customRow);

    QDialogButtonBox* buttons = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel
    );

    QVBoxLayout* root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addSpacing(8);
    root->addWidget(buttons);

    connect(periodCombo, &QComboBox::currentIndexChanged, this, [this]() {
        updatePeriodUi();
    });
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        validateAndAccept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    updatePeriodUi();
}

void BudgetDialog::populateCategories() {
    categoryCombo->clear();
    categoryCombo->addItem("Tất cả chi tiêu", 0);

    categories->getCategories().forEach([&](const Category& category) {
        if (category.getType() != CategoryType::EXPENSE) return;
        categoryCombo->addItem(
            QString::fromStdString(category.getName()),
            category.getId()
        );
    });
}

void BudgetDialog::updatePeriodUi() {
    bool custom = periodCombo->currentData().toInt() == 3;
    referenceRow->setVisible(!custom);
    customRow->setVisible(custom);
}

void BudgetDialog::validateAndAccept() {
    if (startDate() > endDate()) {
        QMessageBox::warning(this, "Ngày không hợp lệ", "Ngày bắt đầu phải trước ngày kết thúc.");
        return;
    }
    accept();
}

double BudgetDialog::amount() const {
    return amountInput->value();
}

int BudgetDialog::categoryId() const {
    return categoryCombo->currentData().toInt();
}

QString BudgetDialog::startDate() const {
    int mode = periodCombo->currentData().toInt();
    QDate ref = referenceDate->date();

    if (mode == 0) {
        int dayOfWeek = ref.dayOfWeek();
        return ref.addDays(1 - dayOfWeek).toString("yyyy-MM-dd");
    }
    if (mode == 1) {
        return QDate(ref.year(), ref.month(), 1).toString("yyyy-MM-dd");
    }
    if (mode == 2) {
        return QDate(ref.year(), 1, 1).toString("yyyy-MM-dd");
    }
    return customStart->date().toString("yyyy-MM-dd");
}

QString BudgetDialog::endDate() const {
    int mode = periodCombo->currentData().toInt();
    QDate ref = referenceDate->date();

    if (mode == 0) {
        int dayOfWeek = ref.dayOfWeek();
        return ref.addDays(7 - dayOfWeek).toString("yyyy-MM-dd");
    }
    if (mode == 1) {
        return QDate(ref.year(), ref.month(), 1)
            .addMonths(1).addDays(-1)
            .toString("yyyy-MM-dd");
    }
    if (mode == 2) {
        return QDate(ref.year(), 12, 31).toString("yyyy-MM-dd");
    }
    return customEnd->date().toString("yyyy-MM-dd");
}
