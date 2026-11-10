#pragma once

#include <QWidget>

class IconBadge : public QWidget {
public:
    explicit IconBadge(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};
