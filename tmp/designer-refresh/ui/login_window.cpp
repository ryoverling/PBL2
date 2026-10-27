#include "login_window.h"
#include "ui/ui_login_window.h"

#include "main_window.h"
#include "data_paths.h"

#include "../model/user.h"
#include "../storage/file_manager.h"

#include <QApplication>
#include <QCheckBox>
#include <QFrame>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

LoginWindow::LoginWindow(QWidget* parent)
    : QWidget(parent), ui(new Ui::LoginWindow) {
    ui->setupUi(this);
    usernameInput = ui->usernameInput;
    passwordInput = ui->passwordInput;
    showPasswordCheck = ui->showPasswordCheck;
    messageLabel = ui->messageLabel;

    connect(ui->loginButton, &QPushButton::clicked, this, [this]() {
        handleLogin();
    });

    connect(passwordInput, &QLineEdit::returnPressed, this, [this]() {
        handleLogin();
    });

    connect(showPasswordCheck, &QCheckBox::toggled, this, [this](bool checked) {
        passwordInput->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    });
    if (!ensureDefaultUser()) {
        messageLabel->setText("Không thể đọc hoặc tạo tài khoản. Hãy kiểm tra thư mục dữ liệu.");
    }
}

LoginWindow::~LoginWindow() {
    delete ui;
}

bool LoginWindow::ensureDefaultUser() {
    if (!FileManager::ensureDataDirectory(DataPaths::dataDir().toStdString()) ||
        QFileInfo::exists(QString::fromStdString(DataPaths::userFile() + ".bak"))) return false;
    User user;
    if (QFileInfo::exists(QString::fromStdString(DataPaths::userFile()))) {
        return FileManager::loadUser(DataPaths::userFile(), user);
    }
    return FileManager::saveUser(DataPaths::userFile(), User("admin", "123456", "Người dùng"));
}

void LoginWindow::handleLogin() {
    User user;
    if (!ensureDefaultUser() || !FileManager::loadUser(DataPaths::userFile(), user)) {
        messageLabel->setText("Không thể đọc dữ liệu người dùng.");
        return;
    }

    string username = usernameInput->text().trimmed().toStdString();
    string password = passwordInput->text().toStdString();

    if (!user.login(username, password)) {
        messageLabel->setText("Tên đăng nhập hoặc mật khẩu không đúng.");
        return;
    }

    messageLabel->clear();

    MainWindow* mainWindow = new MainWindow(
        QString::fromStdString(user.getName()),
        [this]() {
            usernameInput->clear();
            passwordInput->clear();
            messageLabel->clear();
            this->show();
            this->raise();
            this->activateWindow();
        }
    );

    if (!mainWindow->isReady()) {
        delete mainWindow;
        messageLabel->setText("Chưa mở được dữ liệu ứng dụng. Các file hiện có được giữ nguyên.");
        return;
    }
    mainWindow->setAttribute(Qt::WA_DeleteOnClose);
    mainWindow->show();
    hide();
}
