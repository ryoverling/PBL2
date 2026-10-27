#include "transaction_dialog.h"
#include "ui/ui_transaction_dialog.h"
#include "ui_helpers.h"

#include "../manager/wallet_manager.h"
#include "../manager/category_manager.h"
#include "../model/transaction.h"
#include "../model/income.h"
#include "../model/expense.h"
#include "../utils/date_utils.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTextEdit>
#include <QVBoxLayout>

TransactionDialog::TransactionDialog(WalletManager* wallets,
                                     CategoryManager* categories,
                                     const Transaction* existing,
                                     QWidget* parent)
    : QDialog(parent),
      ui(new Ui::TransactionDialog),
      wallets(wallets),
      categories(categories),
      existing(existing) {
    ui->setupUi(this);
    UiHelpers::prepareDialogButtons(ui->buttons);
    setWindowTitle(existing ? "Sửa giao dịch" : "Thêm giao dịch");
    typeCombo = ui->typeCombo;
    amountInput = ui->amountInput;
    categoryCombo = ui->categoryCombo;
    walletCombo = ui->walletCombo;
    noteInput = ui->noteInput;
    timeLabel = ui->timeLabel;
    typeCombo->addItem("Chi tiêu", static_cast<int>(TransactionType::EXPENSE));
    typeCombo->addItem("Thu nhập", static_cast<int>(TransactionType::INCOME));

    populateWallets();

    if (existing) {
        int typeIndex = typeCombo->findData(static_cast<int>(existing->getType()));
        if (typeIndex >= 0) typeCombo->setCurrentIndex(typeIndex);
        populateCategories();

        amountInput->setValue(existing->getAmount());

        int walletIndex = walletCombo->findData(existing->getWalletId());
        if (walletIndex >= 0) walletCombo->setCurrentIndex(walletIndex);

        int categoryIndex = categoryCombo->findData(existing->getCategoryId());
        if (categoryIndex >= 0) categoryCombo->setCurrentIndex(categoryIndex);

        noteInput->setPlainText(QString::fromStdString(existing->getNote()));
        timeLabel->setText(QString::fromStdString(existing->getDateTime()) + " (giữ nguyên)");
    } else {
        populateCategories();
        timeLabel->setText(QString::fromStdString(DateUtils::getCurrentDateTime()) + " (tự động)");
    }

    connect(typeCombo, &QComboBox::currentIndexChanged, this, [this]() {
        populateCategories();
    });

    connect(ui->buttons, &QDialogButtonBox::accepted, this, [this]() {
        validateAndAccept();
    });
    connect(ui->buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

TransactionDialog::~TransactionDialog() {
    delete ui;
}

void TransactionDialog::populateWallets() {
    int previous = walletCombo->currentData().toInt();
    walletCombo->clear();

    wallets->getWallets().forEach([&](const Wallet& wallet) {
        walletCombo->addItem(
            QString::fromStdString(wallet.getName()),
            wallet.getId()
        );
    });

    int index = walletCombo->findData(previous);
    if (index >= 0) walletCombo->setCurrentIndex(index);
}

void TransactionDialog::populateCategories() {
    int previous = categoryCombo->currentData().toInt();
    categoryCombo->clear();

    TransactionType type = static_cast<TransactionType>(typeCombo->currentData().toInt());
    CategoryType wanted = type == TransactionType::INCOME
        ? CategoryType::INCOME
        : CategoryType::EXPENSE;

    categories->getCategories().forEach([&](const Category& category) {
        if (category.getType() != wanted) return;
        categoryCombo->addItem(
            QString::fromStdString(category.getName()),
            category.getId()
        );
    });

    int index = categoryCombo->findData(previous);
    if (index >= 0) categoryCombo->setCurrentIndex(index);
}

void TransactionDialog::validateAndAccept() {
    if (walletCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Thiếu ví", "Vui lòng tạo ít nhất một ví.");
        return;
    }

    if (categoryCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Thiếu danh mục", "Không có danh mục phù hợp.");
        return;
    }

    QString categoryName = categoryCombo->currentText().toLower();
    if ((categoryName.contains("khác") || categoryName.contains("khac")) &&
        noteInput->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(
            this,
            "Cần ghi chú",
            "Với danh mục Khác, vui lòng ghi rõ nội dung giao dịch."
        );
        return;
    }

    accept();
}

Transaction* TransactionDialog::createTransaction(int id) const {
    TransactionType type = static_cast<TransactionType>(typeCombo->currentData().toInt());
    double amount = amountInput->value();
    int walletId = walletCombo->currentData().toInt();
    int categoryId = categoryCombo->currentData().toInt();
    string note = noteInput->toPlainText().trimmed().toStdString();

    string dateTime = existing
        ? existing->getDateTime()
        : DateUtils::getCurrentDateTime();

    if (type == TransactionType::INCOME) {
        return new Income(id, amount, dateTime, note, walletId, categoryId);
    }

    return new Expense(id, amount, dateTime, note, walletId, categoryId);
}
