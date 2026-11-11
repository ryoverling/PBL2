#include "wallet_card.h"
#include "ui_wallet_card.h"
#include "../gui_helpers.h"
#include "../gui_icons.h"
#include "../../core/model/wallet.h"
#include <QMouseEvent>
#include <QResizeEvent>
#include <QStyle>

WalletCard::WalletCard(const Wallet& wallet, QWidget* parent)
    : QFrame(parent), ui(new Ui::WalletCard), id(wallet.getId()),
      fullName(QString::fromStdString(wallet.getName())) {
    ui->setupUi(this);

    QString lower = fullName.toLower();
    QString kind = "bank";
    if (lower.contains("momo") || wallet.getType() == WalletType::EWALLET) {
        kind = "ewallet";
    } else if (lower.contains("tiền mặt") || wallet.getType() == WalletType::CASH) {
        kind = "cash";
    } else if (lower.contains("tp") || lower.contains("tpbank") || lower.contains("tp bank")) {
        kind = "tpbank";
    }

    setProperty("walletType", kind);
    setProperty("selected", false);

    ui->lblCardIcon->setPixmap(GuiIcons::createWalletCardIcon(fullName, (int)wallet.getType(), 42));
    ui->lblName->setText(fullName);

    QString typeStr = (kind == "ewallet") ? "Ví điện tử" : "Ngân hàng";
    if (kind == "cash") typeStr = "Tiền mặt";
    ui->lblType->setText(typeStr);
    ui->lblType->setProperty("walletType", kind);

    ui->lblBalance->setText(UiHelper::formatBalance(wallet.getBalance()));
    ui->lblChevron->setPixmap(GuiIcons::createChevronCircle(32));

    UiHelper::applyCardShadow(this);
    setCursor(Qt::PointingHandCursor);
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
    if (available > 20) {
        ui->lblName->setText(ui->lblName->fontMetrics().elidedText(fullName, Qt::ElideRight, available));
    }
}

void WalletCard::resizeEvent(QResizeEvent* event) {
    QFrame::resizeEvent(event);
    updateName();
}
