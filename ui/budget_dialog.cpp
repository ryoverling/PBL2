#include "budget_dialog.h"
#include "ui/ui_budget_dialog.h"

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
      ui(new Ui::BudgetDialog),
      categories(categories),
      existing(existing) {
    ui->setupUi(this);
    setWindowTitle(existing ? "Sửa ngân sách" : "Tạo ngân sách");
    amountInput = ui->amountInput;
    categoryCombo = ui->categoryCombo;
    periodCombo = ui->periodCombo;
    referenceDate = ui->referenceDate;
    customStart = ui->customStart;
    customEnd = ui->customEnd;
    referenceRow = ui->referenceRow;
    customRow = ui->customRow;
    populateCategories();
    periodCombo->addItem("Theo tuần", 0);
    periodCombo->addItem("Theo tháng", 1);
    periodCombo->addItem("Theo năm", 2);
    periodCombo->addItem("Tùy chọn", 3);

    referenceDate->setDate(QDate::currentDate());
    customStart->setDate(QDate::currentDate());
    customEnd->setDate(QDate::currentDate().addDays(6));

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

    connect(periodCombo, &QComboBox::currentIndexChanged, this, [this]() {
        updatePeriodUi();
    });
    connect(ui->buttons, &QDialogButtonBox::accepted, this, [this]() {
        validateAndAccept();
    });
    connect(ui->buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    updatePeriodUi();
}

BudgetDialog::~BudgetDialog() {
    delete ui;
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
