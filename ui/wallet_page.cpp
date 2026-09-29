#include "wallet_page.h"

#include "wallet_dialog.h"
#include "ui_helpers.h"

#include "../manager/wallet_manager.h"
#include "../manager/transaction_manager.h"

#include <QAbstractItemView>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

WalletPage::WalletPage(WalletManager* wallets,
                       TransactionManager* transactions,
                       std::function<void()> onChanged,
                       QWidget* parent)
    : QWidget(parent),
      wallets(wallets),
      transactions(transactions),
      onChanged(std::move(onChanged)) {

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 28);
    root->setSpacing(16);

    QHBoxLayout* header = new QHBoxLayout();
    QLabel* title = new QLabel("Ví");
    title->setObjectName("pageTitle");
    QPushButton* addButton = new QPushButton("+ Thêm ví");
    addButton->setObjectName("primaryButton");
    header->addWidget(title);
    header->addStretch();
    header->addWidget(addButton);

    QFrame* summary = new QFrame();
    summary->setObjectName("card");
    QVBoxLayout* summaryLayout = new QVBoxLayout(summary);
    QLabel* caption = new QLabel("Tổng tài sản");
    caption->setObjectName("mutedLabel");
    totalLabel = new QLabel("0 đ");
    totalLabel->setStyleSheet("font-size: 30px; font-weight: 800;");
    summaryLayout->addWidget(caption);
    summaryLayout->addWidget(totalLabel);

    QFrame* card = new QFrame();
    card->setObjectName("card");
    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 18, 18, 18);

    QHBoxLayout* actions = new QHBoxLayout();
    QLabel* section = new QLabel("Danh sách ví");
    section->setObjectName("sectionTitle");
    QPushButton* editButton = new QPushButton("Sửa");
    QPushButton* deleteButton = new QPushButton("Xóa");
    deleteButton->setObjectName("dangerButton");
    actions->addWidget(section);
    actions->addStretch();
    actions->addWidget(editButton);
    actions->addWidget(deleteButton);

    table = new QTableWidget(0, 4);
    table->setHorizontalHeaderLabels({"ID", "Tên ví", "Loại", "Số dư"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->setVisible(false);

    cardLayout->addLayout(actions);
    cardLayout->addWidget(table);

    root->addLayout(header);
    root->addWidget(summary);
    root->addWidget(card, 1);

    connect(addButton, &QPushButton::clicked, this, [this]() { addWallet(); });
    connect(editButton, &QPushButton::clicked, this, [this]() { editWallet(); });
    connect(deleteButton, &QPushButton::clicked, this, [this]() { deleteWallet(); });
    connect(table, &QTableWidget::cellDoubleClicked, this, [this](int, int) { editWallet(); });

    refresh();
}

int WalletPage::selectedId() const {
    int row = table->currentRow();
    if (row < 0 || table->item(row, 0) == nullptr) return -1;
    return table->item(row, 0)->data(Qt::UserRole).toInt();
}

void WalletPage::refresh() {
    totalLabel->setText(UiHelpers::money(wallets->getTotalBalance()));
    table->setRowCount(0);

    wallets->getWallets().forEach([&](const Wallet& wallet) {
        int row = table->rowCount();
        table->insertRow(row);

        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(wallet.getId()));
        idItem->setData(Qt::UserRole, wallet.getId());

        table->setItem(row, 0, idItem);
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(wallet.getName())));
        table->setItem(row, 2, new QTableWidgetItem(UiHelpers::walletTypeLabel(wallet.getType())));
        table->setItem(row, 3, new QTableWidgetItem(UiHelpers::money(wallet.getBalance())));
    });
}

void WalletPage::addWallet() {
    WalletDialog dialog(nullptr, this);
    if (dialog.exec() != QDialog::Accepted) return;

    wallets->createWallet(
        dialog.walletName().toStdString(),
        dialog.walletType(),
        dialog.initialBalance()
    );

    if (onChanged) onChanged();
}

void WalletPage::editWallet() {
    int id = selectedId();
    Wallet* wallet = wallets->findById(id);
    if (wallet == nullptr) {
        QMessageBox::information(this, "Ví", "Vui lòng chọn một ví.");
        return;
    }

    WalletDialog dialog(wallet, this);
    if (dialog.exec() != QDialog::Accepted) return;

    wallets->updateWalletName(id, dialog.walletName().toStdString());
    wallet->setType(dialog.walletType());

    if (onChanged) onChanged();
}

void WalletPage::deleteWallet() {
    int id = selectedId();
    Wallet* wallet = wallets->findById(id);
    if (wallet == nullptr) {
        QMessageBox::information(this, "Ví", "Vui lòng chọn một ví.");
        return;
    }

    if (transactions->hasWalletTransactions(id)) {
        QMessageBox::warning(this, "Không thể xóa", "Ví đang có giao dịch.");
        return;
    }

    if (QMessageBox::question(
            this,
            "Xóa ví",
            "Bạn có chắc muốn xóa ví \"" + QString::fromStdString(wallet->getName()) + "\"?"
        ) != QMessageBox::Yes) {
        return;
    }

    wallets->deleteWallet(id);
    if (onChanged) onChanged();
}
