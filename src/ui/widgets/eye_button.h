#pragma once

#include <QPushButton>

// Nút bấm ẩn/hiện số dư tự vẽ hình con mắt bằng QPainter
class EyeButton : public QPushButton {
    Q_OBJECT

public:
    explicit EyeButton(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

