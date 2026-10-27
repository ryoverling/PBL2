#include <QApplication>

#include "ui/login_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("MoneyManager");
    app.setOrganizationName("PBL2 Team");

    LoginWindow loginWindow;
    loginWindow.show();

    return app.exec();
}
