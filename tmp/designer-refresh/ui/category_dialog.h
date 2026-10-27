#pragma once

#include <QDialog>
#include "../model/category.h"

class QLineEdit;
class QComboBox;
namespace Ui { class CategoryDialog; }

class CategoryDialog : public QDialog {
private:
    Ui::CategoryDialog* ui;
    QLineEdit* nameInput;
    QComboBox* typeCombo;
    bool editing;

    void validateAndAccept();

public:
    explicit CategoryDialog(const Category* category = nullptr, QWidget* parent = nullptr);
    ~CategoryDialog();

    QString categoryName() const;
    CategoryType categoryType() const;
};
