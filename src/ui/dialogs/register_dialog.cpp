#include "register_dialog.h"
#include "ui_register_dialog.h"
#include "../gui_helpers.h"
#include "../../core/app/money_manager.h"
#include <filesystem>

RegisterDialog::RegisterDialog(MoneyManager& mm, const std::string& dataDir, QWidget *parent)
    : QDialog(parent), ui(new Ui::RegisterDialog), moneyManager(mm), dataDirectory(dataDir) {
    ui->setupUi(this);
    UiHelper::setupWindowTheme(this);
    ui->lblError->clear();
}

RegisterDialog::~RegisterDialog() { delete ui; }

std::string RegisterDialog::getRegisteredUsername() const { return registeredUsername; }

static bool checkRegistrationFields(Ui::RegisterDialog* ui, const std::string& user,
                                    const std::string& pass, const std::string& confirm) {
    if (user.empty() || pass.empty()) {
        ui->lblError->setText("Tài khoản và mật khẩu không được để trống!");
        return false;
    }
    if (pass != confirm) {
        ui->lblError->setText("Mật khẩu xác nhận không khớp!");
        return false;
    }
    return true;
}

void RegisterDialog::on_btnSave_clicked() {
    std::string user = ui->edtUsername->text().trimmed().toStdString();
    std::string pass = ui->edtPassword->text().toStdString();
    std::string confirm = ui->edtConfirmPassword->text().toStdString();
    std::string name = ui->edtName->text().trimmed().toStdString();

    if (!checkRegistrationFields(ui, user, pass, confirm)) return;

    if (!moneyManager.registerUser(user, pass, name)) {
        ui->lblError->setText("Tên đăng nhập không hợp lệ hoặc đã được sử dụng!");
        return;
    }

    moneyManager.saveUsers(dataDirectory);
    registeredUsername = user;
    accept();
}

void RegisterDialog::on_btnCancel_clicked() {
    reject();
}
