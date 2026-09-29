#include "category_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

CategoryDialog::CategoryDialog(const Category* category, QWidget* parent)
    : QDialog(parent), editing(category != nullptr) {

    setWindowTitle(editing ? "Sửa danh mục" : "Thêm danh mục");
    setMinimumWidth(380);

    nameInput = new QLineEdit();
    nameInput->setPlaceholderText("Ví dụ: Ăn uống");

    typeCombo = new QComboBox();
    typeCombo->addItem("Chi tiêu", static_cast<int>(CategoryType::EXPENSE));
    typeCombo->addItem("Thu nhập", static_cast<int>(CategoryType::INCOME));

    if (category != nullptr) {
        nameInput->setText(QString::fromStdString(category->getName()));
        int target = static_cast<int>(category->getType());
        int index = typeCombo->findData(target);
        if (index >= 0) typeCombo->setCurrentIndex(index);
        typeCombo->setEnabled(false);
    }

    QFormLayout* form = new QFormLayout();
    form->addRow("Tên danh mục", nameInput);
    form->addRow("Loại", typeCombo);

    QDialogButtonBox* buttons = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel
    );

    QVBoxLayout* root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addSpacing(10);
    root->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        validateAndAccept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
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
