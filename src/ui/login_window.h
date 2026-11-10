#pragma once

#include <QDialog>
#include <string>

class MoneyManager;

namespace Ui {
class LoginWindow;
}

// Cửa sổ đăng nhập người dùng
class LoginWindow : public QDialog {
    Q_OBJECT

public:
    explicit LoginWindow(MoneyManager& mm, const std::string& dataDir = "data", QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void on_btnLogin_clicked();
    void on_chkShowPassword_toggled(bool checked);
    void on_btnRegister_clicked();

private:
    Ui::LoginWindow *ui;
    MoneyManager& moneyManager;
    std::string dataDirectory;
};
