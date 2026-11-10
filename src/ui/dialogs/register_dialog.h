#pragma once

#include <QDialog>
#include <string>

namespace Ui { class RegisterDialog; }
class MoneyManager;

// Dialog đăng ký tài khoản mới cho sinh viên / người dùng
class RegisterDialog : public QDialog {
    Q_OBJECT

public:
    explicit RegisterDialog(MoneyManager& mm, const std::string& dataDir, QWidget *parent = nullptr);
    ~RegisterDialog();

    std::string getRegisteredUsername() const;

private slots:
    void on_btnSave_clicked();
    void on_btnCancel_clicked();

private:
    Ui::RegisterDialog *ui;
    MoneyManager& moneyManager;
    std::string dataDirectory;
    std::string registeredUsername;
};

