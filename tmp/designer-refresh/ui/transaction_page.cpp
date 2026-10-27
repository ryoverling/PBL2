#include "transaction_page.h"
#include "ui/ui_transaction_page.h"

#include "transaction_dialog.h"
#include "ui_helpers.h"

#include "../manager/transaction_manager.h"
#include "../manager/wallet_manager.h"
#include "../manager/category_manager.h"
#include "../model/transaction.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>


TransactionPage::TransactionPage(TransactionManager* transactions,
                                 WalletManager* wallets,
                                 CategoryManager* categories,
                                 std::function<void()> onChanged,
                                 QWidget* parent)
    : QWidget(parent),
      ui(new Ui::TransactionPage),
      transactions(transactions),
      wallets(wallets),
      categories(categories),
      onChanged(std::move(onChanged)) {
    ui->setupUi(this);
    searchInput = ui->searchInput;
    typeFilter = ui->typeFilter;
    walletFilter = ui->walletFilter;
    categoryFilter = ui->categoryFilter;
    periodFilter = ui->periodFilter;
    fromDate = ui->fromDate;
    toDate = ui->toDate;
    table = ui->table;
    UiHelpers::bindTableActions(table, ui->editButton, ui->deleteButton, ui->statusLabel, ui->emptyLabel);
    typeFilter->addItem("Tất cả loại", -1);
    typeFilter->addItem("Chi tiêu", static_cast<int>(TransactionType::EXPENSE));
    typeFilter->addItem("Thu nhập", static_cast<int>(TransactionType::INCOME));

    periodFilter->addItem("Tất cả thời gian", 0);
    periodFilter->addItem("Tháng này", 1);
    periodFilter->addItem("Tùy chọn", 2);

    fromDate->setDate(QDate::currentDate().addMonths(-1));
    toDate->setDate(QDate::currentDate());

    connect(ui->addButton, &QPushButton::clicked, this, [this]() { addTransaction(); });
    connect(ui->editButton, &QPushButton::clicked, this, [this]() { editTransaction(); });
    connect(ui->deleteButton, &QPushButton::clicked, this, [this]() { deleteTransaction(); });
    connect(table, &QTableWidget::cellDoubleClicked, this, [this](int, int) { editTransaction(); });

    auto refreshFn = [this]() { refresh(); };
    connect(searchInput, &QLineEdit::textChanged, this, refreshFn);
    connect(typeFilter, &QComboBox::currentIndexChanged, this, refreshFn);
    connect(walletFilter, &QComboBox::currentIndexChanged, this, refreshFn);
    connect(categoryFilter, &QComboBox::currentIndexChanged, this, refreshFn);
    connect(periodFilter, &QComboBox::currentIndexChanged, this, [this]() {
        updatePeriodControls();
        refresh();
    });
    connect(fromDate, &QDateEdit::dateChanged, this, refreshFn);
    connect(toDate, &QDateEdit::dateChanged, this, refreshFn);

    rebuildFilters();
    updatePeriodControls();
    refresh();
}

TransactionPage::~TransactionPage() {
    delete ui;
}

int TransactionPage::selectedId() const {
    int row = table->currentRow();
    if (row < 0 || table->item(row, 0) == nullptr) return -1;
    return table->item(row, 0)->data(Qt::UserRole).toInt();
}

void TransactionPage::rebuildFilters() {
    int oldWallet = walletFilter->currentData().toInt();
    int oldCategory = categoryFilter->currentData().toInt();

    walletFilter->blockSignals(true);
    categoryFilter->blockSignals(true);

    walletFilter->clear();
    walletFilter->addItem("Tất cả ví", 0);
    wallets->getWallets().forEach([&](const Wallet& wallet) {
        walletFilter->addItem(QString::fromStdString(wallet.getName()), wallet.getId());
    });

    categoryFilter->clear();
    categoryFilter->addItem("Tất cả danh mục", 0);
    categories->getCategories().forEach([&](const Category& category) {
        categoryFilter->addItem(QString::fromStdString(category.getName()), category.getId());
    });

    int walletIndex = walletFilter->findData(oldWallet);
    if (walletIndex >= 0) walletFilter->setCurrentIndex(walletIndex);

    int categoryIndex = categoryFilter->findData(oldCategory);
    if (categoryIndex >= 0) categoryFilter->setCurrentIndex(categoryIndex);

    walletFilter->blockSignals(false);
    categoryFilter->blockSignals(false);
}

void TransactionPage::updatePeriodControls() {
    bool custom = periodFilter->currentData().toInt() == 2;
    fromDate->setVisible(custom);
    toDate->setVisible(custom);
    ui->fromLabel->setVisible(custom);
    ui->toLabel->setVisible(custom);
}

bool TransactionPage::passesDateFilter(const std::string& dateTime) const {
    int mode = periodFilter->currentData().toInt();
    if (mode == 0) return true;

    QDate date = QDate::fromString(
        QString::fromStdString(dateTime).left(10),
        "yyyy-MM-dd"
    );
    if (!date.isValid()) return false;

    if (mode == 1) {
        QDate now = QDate::currentDate();
        return date.year() == now.year() && date.month() == now.month();
    }

    return date >= fromDate->date() && date <= toDate->date();
}

void TransactionPage::refresh() {
    rebuildFilters();
    table->setRowCount(0);
    const bool invalidRange = periodFilter->currentData().toInt() == 2 && fromDate->date() > toDate->date();
    ui->errorLabel->setVisible(invalidRange);
    ui->errorLabel->setText(invalidRange ? "Ngày bắt đầu phải trước hoặc bằng ngày kết thúc." : "");
    ui->emptyLabel->setText(invalidRange ? "Hãy kiểm tra lại khoảng thời gian đã chọn." :
        "Không có giao dịch phù hợp. Thử đổi bộ lọc hoặc thêm giao dịch mới.");

    QString query = searchInput->text().trimmed().toLower();
    int type = typeFilter->currentData().toInt();
    int walletId = walletFilter->currentData().toInt();
    int categoryId = categoryFilter->currentData().toInt();

    LinkedList<const Transaction*> items;
    transactions->getTransactionsByDate(items);

    items.forEach([&](const Transaction* t) {
        if (type != -1 && static_cast<int>(t->getType()) != type) return;
        if (walletId != 0 && t->getWalletId() != walletId) return;
        if (categoryId != 0 && t->getCategoryId() != categoryId) return;
        if (!passesDateFilter(t->getDateTime())) return;

        const Category* category = categories->findById(t->getCategoryId());
        const Wallet* wallet = wallets->findById(t->getWalletId());

        QString categoryName = category
            ? QString::fromStdString(category->getName())
            : "Không rõ";
        QString walletName = wallet
            ? QString::fromStdString(wallet->getName())
            : "Không rõ";

        QString searchable = (
            QString::fromStdString(t->getNote()) + " " +
            categoryName + " " + walletName
        ).toLower();

        if (!query.isEmpty() && !searchable.contains(query)) return;

        int row = table->rowCount();
        table->insertRow(row);

        QTableWidgetItem* timeItem = new QTableWidgetItem(
            UiHelpers::dateTimeDisplay(t->getDateTime())
        );
        timeItem->setData(Qt::UserRole, t->getId());

        table->setItem(row, 0, timeItem);
        table->setItem(row, 1, new QTableWidgetItem(
            t->getType() == TransactionType::INCOME ? "Thu nhập" : "Chi tiêu"
        ));
        table->setItem(row, 2, new QTableWidgetItem(categoryName));
        table->setItem(row, 3, new QTableWidgetItem(walletName));
        table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(t->getNote())));
        table->setItem(row, 5, new QTableWidgetItem(UiHelpers::money(t->getSignedAmount())));
    });
    UiHelpers::completeTable(table, {5});
}

void TransactionPage::addTransaction() {
    if (wallets->getWallets().isEmpty()) {
        QMessageBox::warning(this, "Chưa có ví", "Hãy tạo ví trước khi thêm giao dịch.");
        return;
    }

    TransactionDialog dialog(wallets, categories, nullptr, this);
    if (dialog.exec() != QDialog::Accepted) return;

    int id = transactions->getNextId();
    Transaction* transaction = dialog.createTransaction(id);

    if (!transactions->addTransaction(transaction, *wallets, *categories)) {
        delete transaction;
        QMessageBox::warning(this, "Không thể thêm", "Dữ liệu giao dịch không hợp lệ.");
        return;
    }

    if (onChanged) onChanged();
}

void TransactionPage::editTransaction() {
    int id = selectedId();
    Transaction* oldTransaction = transactions->findById(id);
    if (oldTransaction == nullptr) {
        QMessageBox::information(this, "Giao dịch", "Vui lòng chọn một giao dịch.");
        return;
    }

    TransactionDialog dialog(wallets, categories, oldTransaction, this);
    if (dialog.exec() != QDialog::Accepted) return;

    Transaction* replacement = dialog.createTransaction(id);

    if (!transactions->updateTransaction(
            id, replacement, *wallets, *categories
        )) {
        delete replacement;
        QMessageBox::warning(this, "Không thể sửa", "Không thể cập nhật giao dịch.");
        return;
    }

    if (onChanged) onChanged();
}

void TransactionPage::deleteTransaction() {
    int id = selectedId();
    const Transaction* transaction = transactions->findById(id);
    if (transaction == nullptr) {
        QMessageBox::information(this, "Giao dịch", "Vui lòng chọn một giao dịch.");
        return;
    }

    if (QMessageBox::question(
            this,
            "Xóa giao dịch",
            "Bạn có chắc muốn xóa giao dịch đã chọn?"
        ) != QMessageBox::Yes) {
        return;
    }

    if (!transactions->deleteTransaction(id, *wallets)) {
        QMessageBox::warning(this, "Lỗi", "Không thể xóa giao dịch.");
        return;
    }

    if (onChanged) onChanged();
}
