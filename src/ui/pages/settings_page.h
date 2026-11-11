#pragma once

#include <QWidget>
#include <string>

class MoneyManager;
class MainWindow;
class QLabel;

namespace Ui {
class SettingsPage;
}

// Trang Cài đặt: xem tài khoản, đổi mật khẩu, đăng xuất
class SettingsPage : public QWidget {
    Q_OBJECT

public:
    explicit SettingsPage(MoneyManager& mm, const std::string& userDir, const std::string& baseDir, MainWindow* mainWin, QWidget *parent = nullptr);
    ~SettingsPage();

    void refreshData();

private slots:
    void on_btnChangePassword_clicked();
    void on_btnLogout_clicked();
    void on_chkShowPassword_toggled(bool checked);

private:
    Ui::SettingsPage *ui;
    MoneyManager& moneyManager;
    std::string baseDataDirectory;
    MainWindow* mainWindow;
    QLabel* lblPasswordError = nullptr;
    bool validatePasswords();
};
