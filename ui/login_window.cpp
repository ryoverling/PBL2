#include "login_window.h"

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
    : QWidget(parent) {

    setWindowTitle("MoneyManager - Đăng nhập");
    setMinimumSize(760, 520);
    resize(900, 580);

    QHBoxLayout* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    QFrame* brandPanel = new QFrame();
    brandPanel->setStyleSheet(
        "QFrame { background: #17181d; }"
        "QLabel { color: white; background: transparent; }"
    );
    brandPanel->setMinimumWidth(330);

    QVBoxLayout* brandLayout = new QVBoxLayout(brandPanel);
    brandLayout->setContentsMargins(44, 44, 44, 44);

    QLabel* brand = new QLabel("MONEY\nMANAGER");
    brand->setStyleSheet("font-size: 34px; font-weight: 800; color: white;");

    QLabel* slogan = new QLabel(
        "Ghi chép thu chi hằng ngày,\n"
        "theo dõi ví và ngân sách\n"
        "ngay trên máy tính của bạn."
    );
    slogan->setStyleSheet("font-size: 15px; color: #b9bcc5; line-height: 1.5;");

    brandLayout->addStretch();
    brandLayout->addWidget(brand);
    brandLayout->addSpacing(18);
    brandLayout->addWidget(slogan);
    brandLayout->addStretch();

    QWidget* formPanel = new QWidget();
    QVBoxLayout* formLayout = new QVBoxLayout(formPanel);
    formLayout->setContentsMargins(70, 54, 70, 54);

    QLabel* title = new QLabel("Chào mừng trở lại");
    title->setObjectName("pageTitle");

    QLabel* subtitle = new QLabel("Đăng nhập để tiếp tục vào MoneyManager.");
    subtitle->setObjectName("mutedLabel");

    QLabel* usernameLabel = new QLabel("Tên đăng nhập");
    usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("Nhập tên đăng nhập");

    QLabel* passwordLabel = new QLabel("Mật khẩu");
    passwordInput = new QLineEdit();
    passwordInput->setPlaceholderText("Nhập mật khẩu");
    passwordInput->setEchoMode(QLineEdit::Password);

    showPasswordCheck = new QCheckBox("Hiện mật khẩu");

    QPushButton* loginButton = new QPushButton("Đăng nhập");
    loginButton->setObjectName("primaryButton");

    messageLabel = new QLabel();
    messageLabel->setStyleSheet("color: #b42318;");
    messageLabel->setWordWrap(true);

    QLabel* hint = new QLabel("Tài khoản mặc định lần đầu: admin / 123456");
    hint->setObjectName("mutedLabel");
    hint->setStyleSheet("color: #8b8e96; font-size: 12px;");

    formLayout->addStretch();
    formLayout->addWidget(title);
    formLayout->addWidget(subtitle);
    formLayout->addSpacing(28);
    formLayout->addWidget(usernameLabel);
    formLayout->addWidget(usernameInput);
    formLayout->addSpacing(12);
    formLayout->addWidget(passwordLabel);
    formLayout->addWidget(passwordInput);
    formLayout->addWidget(showPasswordCheck);
    formLayout->addSpacing(12);
    formLayout->addWidget(loginButton);
    formLayout->addWidget(messageLabel);
    formLayout->addSpacing(12);
    formLayout->addWidget(hint);
    formLayout->addStretch();

    root->addWidget(brandPanel, 4);
    root->addWidget(formPanel, 6);

    connect(loginButton, &QPushButton::clicked, this, [this]() {
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
