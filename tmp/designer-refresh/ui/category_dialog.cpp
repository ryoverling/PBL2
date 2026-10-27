#include "category_dialog.h"
#include "ui/ui_category_dialog.h"
#include "ui_helpers.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

CategoryDialog::CategoryDialog(const Category* category, QWidget* parent)
    : QDialog(parent), ui(new Ui::CategoryDialog), editing(category != nullptr) {
    ui->setupUi(this);
    UiHelpers::prepareDialogButtons(ui->buttons);
    setWindowTitle(editing ? "Sửa danh mục" : "Thêm danh mục");
    nameInput = ui->nameInput;
    typeCombo = ui->typeCombo;
    typeCombo->addItem("Chi tiêu", static_cast<int>(CategoryType::EXPENSE));
    typeCombo->addItem("Thu nhập", static_cast<int>(CategoryType::INCOME));

    if (category != nullptr) {
        nameInput->setText(QString::fromStdString(category->getName()));
        int target = static_cast<int>(category->getType());
        int index = typeCombo->findData(target);
        if (index >= 0) typeCombo->setCurrentIndex(index);
        typeCombo->setEnabled(false);
    }

    connect(ui->buttons, &QDialogButtonBox::accepted, this, [this]() {
        validateAndAccept();
    });
    connect(ui->buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

CategoryDialog::~CategoryDialog() {
    delete ui;
}

void CategoryDialog::validateAndAccept() {
    if (nameInput->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Thiếu dữ liệu", "Vui lòng nhập tên danh mục.");
        return;
    }
    accept();
}

QString CategoryDialog::categoryName() const {
    return nameInput->text().trimmed();
}

CategoryType CategoryDialog::categoryType() const {
    return static_cast<CategoryType>(typeCombo->currentData().toInt());
}
