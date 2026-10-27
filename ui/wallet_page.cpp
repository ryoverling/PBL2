#include "wallet_page.h"
#include "ui/ui_wallet_page.h"

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
      ui(new Ui::WalletPage),
      wallets(wallets),
      transactions(transactions),
      onChanged(std::move(onChanged)) {
    ui->setupUi(this);
    totalLabel = ui->totalLabel;
    table = ui->table;

    connect(ui->addButton, &QPushButton::clicked, this, [this]() { addWallet(); });
    connect(ui->editButton, &QPushButton::clicked, this, [this]() { editWallet(); });
    connect(ui->deleteButton, &QPushButton::clicked, this, [this]() { deleteWallet(); });
    connect(table, &QTableWidget::cellDoubleClicked, this, [this](int, int) { editWallet(); });

    refresh();
}

WalletPage::~WalletPage() {
    delete ui;
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

    int id = wallets->createWallet(
        dialog.walletName().toStdString(),
        dialog.walletType(),
        dialog.initialBalance()
    );

    if (id < 0) {
        QMessageBox::warning(this, "Không thể thêm", "Dữ liệu ví không hợp lệ.");
        return;
    }

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

    if (!wallets->updateWallet(id, dialog.walletName().toStdString(), dialog.walletType())) {
        QMessageBox::warning(this, "Không thể sửa", "Dữ liệu ví không hợp lệ.");
        return;
    }

    if (onChanged) onChanged();
}

void WalletPage::deleteWallet() {
    int id = selectedId();
    Wallet* wallet = wallets->findById(id);
    if (wallet == nullptr) {
        QMessageBox::information(this, "Ví", "Vui lòng chọn một ví.");
        return;
    }

    if (QMessageBox::question(
            this,
            "Xóa ví",
            "Bạn có chắc muốn xóa ví \"" + QString::fromStdString(wallet->getName()) + "\"?"
        ) != QMessageBox::Yes) {
        return;
    }

    if (!wallets->deleteWallet(id, *transactions)) {
        QMessageBox::warning(this, "Không thể xóa", "Ví đang có giao dịch.");
        return;
    }
    if (onChanged) onChanged();
}
