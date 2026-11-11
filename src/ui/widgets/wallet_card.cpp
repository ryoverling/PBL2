#include "wallet_card.h"
#include "ui_wallet_card.h"
#include "../gui_helpers.h"
#include "../gui_icons_wallet.h"
#include "../../core/model/wallet.h"
#include <QMouseEvent>
#include <QResizeEvent>
#include <QStyle>
#include <QPainter>
#include <QPen>

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

AddWalletCard::AddWalletCard(QWidget* parent) : QPushButton("+ Thêm ví", parent) {
    setObjectName("addWalletCard");
    setMinimumSize(260, 150);
    setMaximumHeight(165);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover);
}

void AddWalletCard::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    bool hover = underMouse() || isDown();
    p.setBrush(QColor(hover ? "#F4F8FE" : "#FFFFFF"));
    p.setPen(QPen(QColor(hover ? "#1E6FFF" : "#BACFE8"), 1.6, Qt::DashLine));
    p.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 18, 18);

    QPoint center(width() / 2, height() / 2 - 14);

    // 3 tia sáng phía trên nút tròn
    p.setPen(QPen(QColor("#1E6FFF"), 1.8, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(center.x() - 14, center.y() - 28, center.x() - 9, center.y() - 24);
    p.drawLine(center.x(), center.y() - 32, center.x(), center.y() - 26);
    p.drawLine(center.x() + 14, center.y() - 28, center.x() + 9, center.y() - 24);

    // Nút tròn nền xanh pastel
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(hover ? "#CFE2FF" : "#E0EDFF"));
    p.drawEllipse(center, 22, 22);

    // Dấu cộng xanh dương
    p.setPen(QPen(QColor("#1E6FFF"), 2.4, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(center.x() - 8, center.y(), center.x() + 8, center.y());
    p.drawLine(center.x(), center.y() - 8, center.x(), center.y() + 8);

    // Text "+ Thêm ví"
    QFont font = p.font();
    font.setPixelSize(14);
    font.setBold(true);
    p.setFont(font);
    p.setPen(QColor("#0B2A6B"));
    p.drawText(QRect(0, center.y() + 30, width(), 24), Qt::AlignCenter, "+ Thêm ví");
}
