#include "wallet_page.h"
#include "ui_wallet_page.h"
#include "../gui_helpers.h"
#include "../main_window.h"
#include "../widgets/wallet_card.h"
#include "../widgets/add_wallet_card.h"
#include "../../core/app/money_manager.h"
#include <QResizeEvent>
#include <QScrollArea>
#include <QEvent>

WalletPage::WalletPage(MoneyManager& mm, const std::string& dataDir, MainWindow* mainWin, QWidget *parent)
    : QWidget(parent), ui(new Ui::WalletPage), moneyManager(mm), dataDirectory(dataDir), mainWindow(mainWin) {
    ui->setupUi(this);
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
    if (watched == ui->scrollWallets->viewport() && event->type() == QEvent::Resize) reflowCards();
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
