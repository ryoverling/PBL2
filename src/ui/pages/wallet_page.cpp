#include "wallet_page.h"
#include "ui_wallet_page.h"
#include "../gui_helpers.h"
#include "../gui_icons_wallet.h"
#include "../main_window.h"
#include "../widgets/wallet_card.h"
#include "../dialogs/wallet_dialog.h"
#include "../../core/app/money_manager.h"
#include <QResizeEvent>
#include <QEvent>

WalletPage::WalletPage(MoneyManager& mm, const std::string& dataDir, MainWindow* mainWin, QWidget *parent)
    : QWidget(parent), ui(new Ui::WalletPage), moneyManager(mm), dataDirectory(dataDir), mainWindow(mainWin) {
    ui->setupUi(this);

    // Gắn icon tiêu đề trang và đổ bóng
    ui->lblHeaderIcon->setPixmap(GuiIcons::createWalletPageHeaderIcon(44));
    UiHelper::applyCardShadow(ui->cardBalance);
    ui->lblHeroIllustration->setPixmap(GuiIcons::createHeroWalletIllustration(130, 95));

    // Thiết lập các nút hành động
    ui->btnAdd->setText(QString::fromUtf8("Thêm ví"));
    ui->btnAdd->setIcon(UiHelper::createPlusIcon(Qt::white));
    ui->btnAdd->setIconSize(QSize(14, 14));
    ui->btnAdd->setCursor(Qt::PointingHandCursor);

    ui->btnEdit->setIcon(UiHelper::createEditIcon(QColor("#0B2A6B")));
    ui->btnEdit->setIconSize(QSize(16, 16));
    ui->btnEdit->setToolTip("Chỉnh sửa ví đang chọn");
    ui->btnEdit->setCursor(Qt::PointingHandCursor);

    ui->btnDelete->setIcon(UiHelper::createTrashIcon(QColor("#E0173D")));
    ui->btnDelete->setIconSize(QSize(16, 16));
    ui->btnDelete->setToolTip("Xóa ví đang chọn");
    ui->btnDelete->setCursor(Qt::PointingHandCursor);

    addCard = new AddWalletCard(ui->walletContainer);
    connect(addCard, &QPushButton::clicked, this, &WalletPage::on_btnAdd_clicked);
    ui->scrollWallets->viewport()->installEventFilter(this);

    refreshData();
}

WalletPage::~WalletPage() { delete ui; }

int WalletPage::getSelectedWalletId() const { return selectedWalletId; }

void WalletPage::onWalletCardClicked(int walletId) {
    selectedWalletId = walletId;
    updateSelection();
}

void WalletPage::updateSelection() {
    bool found = false;
    for (WalletCard* card : cards) {
        bool selected = card->walletId() == selectedWalletId;
        card->setSelected(selected);
        found |= selected;
    }
    if (!found) selectedWalletId = -1;
    ui->btnEdit->setEnabled(found);
    ui->btnDelete->setEnabled(found);
}

void WalletPage::refreshData() {
    populateCards();
    updateSelection();
}

void WalletPage::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    reflowCards();
}

bool WalletPage::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->scrollWallets->viewport() && event->type() == QEvent::Resize) {
        reflowCards();
    }
    return QWidget::eventFilter(watched, event);
}

void WalletPage::on_btnEye_clicked() {
    UiHelper::balanceHidden = ui->btnEye->isChecked();
    if (mainWindow) mainWindow->refreshAllPages();
    else refreshData();
}

void WalletPage::on_btnEmptyAdd_clicked() { on_btnAdd_clicked(); }

void WalletPage::saveAndRefresh() {
    bool saved = moneyManager.saveAll(dataDirectory);
    if (!saved) UiHelper::showError(this, "Không lưu được dữ liệu.");
    if (mainWindow) mainWindow->refreshAllPages();
    else refreshData();
    if (saved) UiHelper::showInfo(this, "Đã lưu ví.");
}

void WalletPage::on_btnAdd_clicked() {
    WalletDialog dialog(moneyManager, -1, this);
    if (dialog.exec() != QDialog::Accepted) return;
    if (moneyManager.addWallet(dialog.getName(), dialog.getType(), dialog.getBalance())) {
        saveAndRefresh();
    } else {
        UiHelper::showError(this, "Không thể thêm ví. Kiểm tra lại tên ví.");
    }
}

void WalletPage::on_btnEdit_clicked() {
    int id = getSelectedWalletId();
    if (id < 0) return;
    WalletDialog dialog(moneyManager, id, this);
    if (dialog.exec() != QDialog::Accepted) return;
    if (moneyManager.updateWallet(id, dialog.getName(), dialog.getType())) {
        saveAndRefresh();
    } else {
        UiHelper::showError(this, "Không thể sửa ví. Kiểm tra lại tên ví.");
    }
}

void WalletPage::on_btnDelete_clicked() {
    int id = getSelectedWalletId();
    if (id < 0) return;
    const Wallet* wallet = moneyManager.getWalletManager().findById(id);
    if (!wallet) return;
    QString name = QString::fromStdString(wallet->getName());
    if (!UiHelper::askConfirmation(this, "Xác nhận xóa ví", "Bạn có chắc muốn xóa ví \"" + name + "\"?")) return;
    if (moneyManager.deleteWallet(id)) {
        saveAndRefresh();
    } else {
        UiHelper::showError(this, "Ví đang có giao dịch, không thể xóa.");
    }
}

void WalletPage::addWalletCard(const Wallet& wallet) {
    auto* card = new WalletCard(wallet, ui->walletContainer);
    connect(card, &WalletCard::clicked, this, &WalletPage::onWalletCardClicked);
    cards.append(card);
}

void WalletPage::populateCards() {
    ui->walletGrid->removeWidget(addCard);
    for (WalletCard* card : cards) {
        ui->walletGrid->removeWidget(card);
        delete card;
    }
    cards.clear();

    const auto& wallets = moneyManager.getWalletManager().getWallets();
    for (const Node<Wallet>* cur = wallets.getHead(); cur; cur = cur->next) {
        addWalletCard(cur->data);
    }

    ui->lblWalletCount->setText(QString::number(cards.size()) + " ví");
    ui->lblTotalBalance->setText(UiHelper::formatBalance(moneyManager.getWalletManager().getTotalBalance()));
    ui->btnEye->setChecked(UiHelper::balanceHidden);
    ui->scrollWallets->setVisible(!cards.isEmpty());
    ui->emptyWallets->setVisible(cards.isEmpty());
    reflowCards();
}

void WalletPage::reflowCards() {
    if (cards.isEmpty() || !isVisible() || reflowing) return;
    reflowing = true;
    int available = ui->scrollWallets->viewport()->width();
    int columns = qBound(1, (available + 20) / 290, 3);

    ui->walletGrid->removeWidget(addCard);
    for (WalletCard* card : cards) ui->walletGrid->removeWidget(card);

    for (int i = 0; i < cards.size(); ++i) {
        ui->walletGrid->addWidget(cards.at(i), i / columns, i % columns);
    }
    ui->walletGrid->addWidget(addCard, cards.size() / columns, cards.size() % columns);

    for (int col = 0; col < columns; ++col) {
        ui->walletGrid->setColumnStretch(col, 1);
    }
    reflowing = false;
}
