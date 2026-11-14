#include "settings_page.h"
#include "ui_settings_page.h"
#include "../main_window.h"
#include "../gui_helpers.h"
#include "../gui_icons_nav.h"
#include "../../core/app/money_manager.h"
#include <QMessageBox>
#include <QPushButton>
#include <QPainter>

// Tạo ảnh đại diện hình vuông bo góc với gradient xanh nhạt
static QPixmap createAvatarPixmap(const QString& username, int size = 54) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QLinearGradient grad(0, 0, size, size);
    grad.setColorAt(0, QColor("#E0EDFF"));
    grad.setColorAt(1, QColor("#C7DEFF"));
    p.setBrush(grad);
    p.setPen(QPen(QColor("#B8D5FA"), 1.5));
    // Bo góc 14px tạo hình vuông bo góc hiện đại (không tròn)
    p.drawRoundedRect(QRectF(1, 1, size - 2, size - 2), 14, 14);

    p.setPen(QColor("#1E6FFF"));
    QFont f("Segoe UI", 18, QFont::Bold);
    p.setFont(f);
    QString letter = username.isEmpty() ? "?" : username.left(1).toUpper();
    p.drawText(pix.rect(), Qt::AlignCenter, letter);
    return pix;
}

// Cài đặt nút mắt xem mật khẩu cho từng ô nhập
static void setupPasswordEyeAction(QLineEdit* edt) {
    QAction* act = edt->addAction(QIcon(GuiIcons::createEyeActionIcon(true)), QLineEdit::TrailingPosition);
    QObject::connect(act, &QAction::triggered, edt, [edt, act]() {
        bool isHidden = (edt->echoMode() == QLineEdit::Password);
        edt->setEchoMode(isHidden ? QLineEdit::Normal : QLineEdit::Password);
        act->setIcon(QIcon(GuiIcons::createEyeActionIcon(!isHidden)));
    });
}

void SettingsPage::setupCards() {
    QWidget* cards[] = {ui->cardAccount, ui->cardUtilities, ui->cardChangePassword};
    for (QWidget* c : cards) {
        c->setProperty("card", true);
        c->setAttribute(Qt::WA_StyledBackground, true);
    }
    UiHelper::applyCardShadows({ui->cardAccount, ui->cardUtilities, ui->cardChangePassword});

    ui->btnLogout->setIcon(QIcon(GuiIcons::createLogoutIcon(16, QColor("#E0173D"))));
    ui->btnChangePassword->setIcon(QIcon(GuiIcons::createLockActionIcon(16, Qt::white)));
    ui->btnChangePassword->setText("Đổi mật khẩu →");
    ui->lblAccountHeading->setText("👤  Tài khoản");
    ui->lblSessionHeading->setText("🛡️  Phiên làm việc");
    ui->lblPasswordHeading->setText("🔒  Đổi mật khẩu");
}

void SettingsPage::setupPasswordForm() {
    ui->lblNewPassword->setText("Mật khẩu mới <span style=\"color:#E0173D\">*</span>");
    ui->lblConfirmPassword->setText("Xác nhận mật khẩu mới <span style=\"color:#E0173D\">*</span>");
    setupPasswordEyeAction(ui->edtOldPassword);
    setupPasswordEyeAction(ui->edtNewPassword);
    setupPasswordEyeAction(ui->edtConfirmPassword);

    lblPasswordError = new QLabel(this);
    lblPasswordError->setStyleSheet("color: #E0173D; font-size: 13px; font-weight: 500;");
    lblPasswordError->setVisible(false);
    ui->changePassLayout->insertWidget(ui->changePassLayout->count() - 1, lblPasswordError);

    connect(ui->edtOldPassword, &QLineEdit::textChanged, this, &SettingsPage::clearPasswordErrors);
    connect(ui->edtNewPassword, &QLineEdit::textChanged, this, &SettingsPage::clearPasswordErrors);
    connect(ui->edtConfirmPassword, &QLineEdit::textChanged, this, &SettingsPage::clearPasswordErrors);
}

void SettingsPage::clearPasswordErrors() {
    if (lblPasswordError) lblPasswordError->setVisible(false);
    ui->edtOldPassword->setStyleSheet("");
    ui->edtNewPassword->setStyleSheet("");
    ui->edtConfirmPassword->setStyleSheet("");
}

SettingsPage::SettingsPage(MoneyManager& mm, const std::string&, const std::string& baseDir, MainWindow* mainWin, QWidget *parent)
    : QWidget(parent), ui(new Ui::SettingsPage), moneyManager(mm),
      baseDataDirectory(baseDir), mainWindow(mainWin) {
    ui->setupUi(this);
    setupCards();
    setupPasswordForm();
    refreshData();
}

SettingsPage::~SettingsPage() {
    delete ui;
}

void SettingsPage::refreshData() {
    QString username = QString::fromStdString(moneyManager.getCurrentUsername());
    ui->lblCurrentAccount->setText(username);
    ui->lblCurrentAccount->setToolTip(username);
    ui->lblAccountAvatar->setPixmap(createAvatarPixmap(username, 52));
    ui->edtOldPassword->clear();
    ui->edtNewPassword->clear();
    ui->edtConfirmPassword->clear();
    ui->chkShowPassword->setChecked(false);
    on_chkShowPassword_toggled(false);
}

bool SettingsPage::validatePasswords() {
    if (ui->edtOldPassword->text().isEmpty()) {
        ui->edtOldPassword->setStyleSheet("QLineEdit { border: 1.5px solid #E0173D; }");
        lblPasswordError->setText("Vui lòng nhập mật khẩu hiện tại.");
        lblPasswordError->setVisible(true);
        UiHelper::showError(this, "Vui lòng nhập đầy đủ thông tin.");
        return false;
    }
    if (ui->edtNewPassword->text().isEmpty()) {
        ui->edtNewPassword->setStyleSheet("QLineEdit { border: 1.5px solid #E0173D; }");
        lblPasswordError->setText("Vui lòng nhập mật khẩu mới.");
        lblPasswordError->setVisible(true);
        UiHelper::showError(this, "Vui lòng nhập đầy đủ thông tin.");
        return false;
    }
    if (ui->edtNewPassword->text().size() < 4) {
        ui->edtNewPassword->setStyleSheet("QLineEdit { border: 1.5px solid #E0173D; }");
        lblPasswordError->setText("Mật khẩu mới cần ít nhất 4 ký tự.");
        lblPasswordError->setVisible(true);
        UiHelper::showError(this, "Mật khẩu mới cần ít nhất 4 ký tự.");
        return false;
    }
    if (ui->edtNewPassword->text() != ui->edtConfirmPassword->text()) {
        ui->edtConfirmPassword->setStyleSheet("QLineEdit { border: 1.5px solid #E0173D; }");
        lblPasswordError->setText("Mật khẩu nhập lại không khớp.");
        lblPasswordError->setVisible(true);
        UiHelper::showError(this, "Mật khẩu nhập lại không khớp.");
        return false;
    }
    lblPasswordError->setVisible(false);
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
