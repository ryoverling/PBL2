#pragma once

#include <QWidget>

class QLineEdit;
class QLabel;
class QCheckBox;

class LoginWindow : public QWidget {
private:
    QLineEdit* usernameInput;
    QLineEdit* passwordInput;
    QLabel* messageLabel;
    QCheckBox* showPasswordCheck;

    void handleLogin();
    bool ensureDefaultUser();

public:
    explicit LoginWindow(QWidget* parent = nullptr);
};
