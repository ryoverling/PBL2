#include "wallet_card.h"
#include "ui_wallet_card.h"
#include "../gui_helpers.h"
#include "../../core/model/wallet.h"
#include <QMouseEvent>
#include <QResizeEvent>
#include <QStyle>

WalletCard::WalletCard(const Wallet& wallet, QWidget* parent)
    : QFrame(parent), ui(new Ui::WalletCard), id(wallet.getId()),
      fullName(QString::fromStdString(wallet.getName())) {
    ui->setupUi(this);
    QString kind = wallet.getType() == WalletType::BANK ? "bank"
                 : wallet.getType() == WalletType::EWALLET ? "ewallet" : "cash";
    setProperty("walletType", kind);
    setProperty("selected", false);
    ui->iconBadge->setProperty("kind", kind);
    ui->lblName->setText(fullName);
    ui->lblType->setText(QString::fromStdString(walletTypeToString(wallet.getType())));
    ui->lblType->setProperty("walletType", kind);
    ui->lblType->setFixedWidth(ui->lblType->fontMetrics().horizontalAdvance(ui->lblType->text()) + 16);
    ui->lblBalance->setText(UiHelper::formatBalance(wallet.getBalance()));
    bool negative = wallet.getBalance() < 0;
    ui->lblBalance->setProperty("negative", negative);
    if (negative) {
        ui->lblWarning->setText(QString::fromUtf8("⚠ Số dư âm, cần đối soát dữ liệu"));
        ui->lblWarning->setStyleSheet("color: #EF4765; font-size: 11px; font-weight: 600;");
        ui->lblWarning->setVisible(true);
    } else {
        ui->lblWarning->setVisible(false);
    }
    setToolTip(fullName);
}

WalletCard::~WalletCard() { delete ui; }

int WalletCard::walletId() const { return id; }

void WalletCard::setSelected(bool selected) {
    if (property("selected").toBool() == selected) return;
    setProperty("selected", selected);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void WalletCard::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) emit clicked(id);
    QFrame::mousePressEvent(event);
}

void WalletCard::updateName() {
    int available = ui->lblName->width();
    if (available > 20) ui->lblName->setText(ui->lblName->fontMetrics().elidedText(fullName, Qt::ElideRight, available));
}

void WalletCard::resizeEvent(QResizeEvent* event) {
    QFrame::resizeEvent(event);
    updateName();
}
