#pragma once

#include <QDialog>
#include "../model/category.h"

class QLineEdit;
class QComboBox;

class CategoryDialog : public QDialog {
private:
    QLineEdit* nameInput;
    QComboBox* typeCombo;
    bool editing;

    void validateAndAccept();

public:
    explicit CategoryDialog(const Category* category = nullptr, QWidget* parent = nullptr);

    QString categoryName() const;
    CategoryType categoryType() const;
};
