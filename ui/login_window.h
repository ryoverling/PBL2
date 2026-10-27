#pragma once

#include <QWidget>

class QLineEdit;
class QLabel;
class QCheckBox;
namespace Ui { class LoginWindow; }

class LoginWindow : public QWidget {
private:
    Ui::LoginWindow* ui;
    QLineEdit* usernameInput;
    QLineEdit* passwordInput;
    QLabel* messageLabel;
    QCheckBox* showPasswordCheck;

    void handleLogin();
    bool ensureDefaultUser();

public:
    explicit LoginWindow(QWidget* parent = nullptr);
    ~LoginWindow();
};
