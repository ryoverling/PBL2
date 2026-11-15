#pragma once

#include <QFrame>
#include <QPushButton>
#include <QString>
#include <QSize>

class Wallet;
class QMouseEvent;
class QResizeEvent;
class QPaintEvent;

namespace Ui { class WalletCard; }

class WalletCard : public QFrame {
    Q_OBJECT
public:
    explicit WalletCard(const Wallet& wallet, QWidget* parent = nullptr);
    ~WalletCard();
    int walletId() const;
    void setSelected(bool selected);
    QSize sizeHint() const override;

signals:
    void clicked(int walletId);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    Ui::WalletCard* ui;
    int id;
    QString fullName;
    void updateName();
};

class AddWalletCard : public QPushButton {
public:
    explicit AddWalletCard(QWidget* parent = nullptr);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
protected:
    void paintEvent(QPaintEvent* event) override;
};
