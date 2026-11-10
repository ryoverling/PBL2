#include "settings_page.h"
#include "ui_settings_page.h"
#include "../main_window.h"
#include "../gui_helpers.h"
#include "../../core/app/money_manager.h"
#include <QMessageBox>
#include <QPushButton>

SettingsPage::SettingsPage(MoneyManager& mm, const std::string&, const std::string& baseDir, MainWindow* mainWin, QWidget *parent)
    : QWidget(parent), ui(new Ui::SettingsPage), moneyManager(mm),
      baseDataDirectory(baseDir), mainWindow(mainWin) {
    ui->setupUi(this);
    refreshData();
}

SettingsPage::~SettingsPage() {
    delete ui;
}

void SettingsPage::refreshData() {
    QString username = QString::fromStdString(moneyManager.getCurrentUsername());
    ui->lblCurrentAccount->setText(username);
    ui->lblCurrentAccount->setToolTip(username);
    ui->lblAccountAvatar->setText(username.isEmpty() ? "?" : username.left(1).toUpper());
    ui->edtOldPassword->clear();
    ui->edtNewPassword->clear();
    ui->edtConfirmPassword->clear();
    ui->chkShowPassword->setChecked(false);
    on_chkShowPassword_toggled(false);
}

bool SettingsPage::validatePasswords() {
    if (ui->edtOldPassword->text().isEmpty() || ui->edtNewPassword->text().isEmpty() ||
        ui->edtConfirmPassword->text().isEmpty()) {
        UiHelper::showError(this, "Vui lòng nhập đầy đủ thông tin.");
        return false;
    }
    if (ui->edtNewPassword->text() != ui->edtConfirmPassword->text()) {
        UiHelper::showError(this, "Mật khẩu nhập lại không khớp.");
        return false;
    }
    if (ui->edtNewPassword->text().size() < 4) {
        UiHelper::showError(this, "Mật khẩu mới cần ít nhất 4 ký tự.");
        return false;
    }
    return true;
}

void SettingsPage::on_chkShowPassword_toggled(bool checked) {
    QLineEdit::EchoMode mode = checked ? QLineEdit::Normal : QLineEdit::Password;
    ui->edtOldPassword->setEchoMode(mode);
    ui->edtNewPassword->setEchoMode(mode);
    ui->edtConfirmPassword->setEchoMode(mode);
}

void SettingsPage::on_btnChangePassword_clicked() {
    if (!validatePasswords()) return;
    std::string oldPass = ui->edtOldPassword->text().toStdString();
    std::string newPass = ui->edtNewPassword->text().toStdString();
    if (!moneyManager.changePassword(moneyManager.getCurrentUsername(), oldPass, newPass)) {
        UiHelper::showError(this, "Mật khẩu cũ sai hoặc mật khẩu mới không hợp lệ.");
        return;
    }
    if (!moneyManager.saveUsers(baseDataDirectory)) {
        moneyManager.changePassword(moneyManager.getCurrentUsername(), newPass, oldPass);
        UiHelper::showError(this, "Không lưu được mật khẩu. Vui lòng thử lại.");
        return;
    }
    refreshData();
    UiHelper::showInfo(this, "Đổi mật khẩu thành công.");
}

void SettingsPage::on_btnLogout_clicked() {
    QMessageBox box(QMessageBox::Question, "Xác nhận đăng xuất",
        "Bạn có chắc chắn muốn đăng xuất khỏi tài khoản hiện tại?", QMessageBox::NoButton, this);
    QPushButton* cancel = box.addButton("Hủy", QMessageBox::RejectRole);
    QPushButton* logout = box.addButton("Đăng xuất", QMessageBox::AcceptRole);
    cancel->setProperty("role", "secondary");
    logout->setProperty("role", "primary");
    box.setDefaultButton(cancel);
    box.exec();
    if (box.clickedButton() != logout) return;
    moneyManager.logout();
    UiHelper::balanceHidden = true;
    if (mainWindow) mainWindow->performLogout();
}
