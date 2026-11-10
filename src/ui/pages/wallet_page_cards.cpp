#include "wallet_page.h"
#include "ui_wallet_page.h"
#include "../gui_helpers.h"
#include "../widgets/wallet_card.h"
#include "../widgets/add_wallet_card.h"
#include "../../core/app/money_manager.h"
#include <QGridLayout>
#include <QScrollArea>

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
    for (const Node<Wallet>* cur = wallets.getHead(); cur; cur = cur->next) addWalletCard(cur->data);
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
    int columns = qBound(1, (available + 20) / 296, 4);
    ui->walletGrid->removeWidget(addCard);
    for (WalletCard* card : cards) ui->walletGrid->removeWidget(card);
    for (int i = 0; i < cards.size(); ++i)
        ui->walletGrid->addWidget(cards.at(i), i / columns, i % columns);
    ui->walletGrid->addWidget(addCard, cards.size() / columns, cards.size() % columns);
    for (int column = 0; column <= cards.size(); ++column)
        ui->walletGrid->setColumnStretch(column, column < columns ? 1 : 0);
    reflowing = false;
}
