#include <QApplication>

#include "ui/login_window.h"
#include "ui/theme.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("MoneyManager");
    app.setOrganizationName("PBL2 Team");
    app.setStyleSheet(Theme::appStyleSheet());

    LoginWindow loginWindow;
    loginWindow.show();

    return app.exec();
}
