#include "login_window.h"
#include "ui_login_window.h"
#include "gui_helpers.h"
#include "gui_icons_nav.h"
#include "gui_icons_wallet.h"
#include "../core/app/money_manager.h"
#include "dialogs/register_dialog.h"

LoginWindow::LoginWindow(MoneyManager& mm, const std::string& dataDir, QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginWindow), moneyManager(mm), dataDirectory(dataDir) {
    ui->setupUi(this);
    UiHelper::setupWindowTheme(this);
    ui->lblError->clear();

    // Hinh minh hoa vi tien & icons giao dien dang nhap
    ui->lblWelcomeArt->setPixmap(GuiIcons::createLoginIllustration(200, 120));
    ui->edtUsername->addAction(GuiIcons::createUserIcon(QColor("#5B6B8C")), QLineEdit::LeadingPosition);
    ui->edtPassword->addAction(GuiIcons::createLockIcon(QColor("#5B6B8C")), QLineEdit::LeadingPosition);
    ui->btnLogin->setIcon(GuiIcons::createRightArrowIcon(Qt::white));
    ui->btnRegister->setIcon(GuiIcons::createKeyIcon(QColor("#1E6FFF")));
}

LoginWindow::~LoginWindow() {
    delete ui;
}

void LoginWindow::on_chkShowPassword_toggled(bool checked) {
    ui->edtPassword->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

void LoginWindow::on_btnLogin_clicked() {
    std::string user = ui->edtUsername->text().trimmed().toStdString();
    std::string pass = ui->edtPassword->text().toStdString();
    if (user.empty() || pass.empty()) {
        ui->lblError->setText("Vui lòng nhập đầy đủ tài khoản và mật khẩu!");
        return;
    }
    if (!moneyManager.login(user, pass)) {
        ui->lblError->setText("Tài khoản hoặc mật khẩu không chính xác!");
        return;
    }
    accept();
}

void LoginWindow::on_btnRegister_clicked() {
    RegisterDialog regDialog(moneyManager, dataDirectory, this);
    if (regDialog.exec() == QDialog::Accepted) {
        ui->edtUsername->setText(QString::fromStdString(regDialog.getRegisteredUsername()));
        ui->edtPassword->clear();
        ui->lblError->setText("Đăng ký thành công! Hãy nhập mật khẩu để đăng nhập.");
    }
}
