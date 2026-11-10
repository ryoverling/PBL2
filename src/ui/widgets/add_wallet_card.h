#pragma once
#include <QPushButton>

class AddWalletCard : public QPushButton {
public:
    explicit AddWalletCard(QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent* event) override;
};
