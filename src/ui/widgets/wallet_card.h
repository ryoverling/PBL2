#pragma once

#include <QFrame>
#include <QString>

class Wallet;
class QMouseEvent;
class QResizeEvent;

namespace Ui { class WalletCard; }

class WalletCard : public QFrame {
    Q_OBJECT
public:
    explicit WalletCard(const Wallet& wallet, QWidget* parent = nullptr);
    ~WalletCard();
    int walletId() const;
    void setSelected(bool selected);

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
