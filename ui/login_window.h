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
    void ensureDefaultUser();

public:
    explicit LoginWindow(QWidget* parent = nullptr);
};
