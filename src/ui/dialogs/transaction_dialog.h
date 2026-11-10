#pragma once

#include <QDialog>
#include <string>
#include "../../core/utils/utils.h"
#include "../../core/model/transaction.h"

class MoneyManager;

namespace Ui {
class TransactionDialog;
}

// Hộp thoại Thêm và Sửa Giao Dịch
class TransactionDialog : public QDialog {
    Q_OBJECT

public:
    explicit TransactionDialog(MoneyManager& mm, int txId = -1, QWidget *parent = nullptr);
    ~TransactionDialog() override;

    void setInitialType(TransactionType type);
    TransactionType getType() const;
    long long getAmount() const;
    Date getDate() const;
    int getWalletId() const;
    std::string getCategoryName() const { return resolvedCategoryName; }
    std::string getNote() const;
    class Transaction* createTransaction(int id, int categoryId = 0) const;

private slots:
    void on_btnTypeExpense_clicked();
    void on_btnTypeIncome_clicked();
    void on_cboCategory_editTextChanged(const QString &text);
    void on_btnSave_clicked();
    void on_btnCancel_clicked();
    void validateLiveInput();

private:
    Ui::TransactionDialog *ui;
    MoneyManager& moneyManager;
    int editingTxId;
    std::string resolvedCategoryName;
    class QButtonGroup* typeGroup = nullptr;

    void updateSegmentedStyle();
    void loadWallets();
    void loadCategories();
    void setupEditData(const class Transaction* tx);
    const class Category* findCategory(const QString& name) const;
};
