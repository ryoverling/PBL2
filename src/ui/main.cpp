#include <QApplication>
#include <QFile>
#include <QFont>
#include <QPalette>
#include <filesystem>
#include "../core/app/money_manager.h"
#include "login_window.h"
#include "main_window.h"
#include "gui_helpers.h"
#include "dialogs/day_detail_dialog.h"
#include "dialogs/transaction_dialog.h"
#include "dialogs/wallet_dialog.h"
#include "dialogs/plan_dialog.h"
#include "dialogs/register_dialog.h"
#include "widgets/date_picker_popup.h"
#include <QDir>
#include <iostream>

namespace fs = std::filesystem;

#ifndef DATA_DIR
#define DATA_DIR "data"
#endif

static void initStyle(QApplication& app) {
    // Chuan hoa font Segoe UI ho tro tieng Viet day du
    QFont appFont("Segoe UI", 10);
    appFont.setStyleHint(QFont::SansSerif);
    app.setFont(appFont);

    // Chuan hoa bang mau nen va mau nhan dong bo he thong (#1E6FFF, #0B2A6B)
    QPalette palette = app.palette();
    palette.setColor(QPalette::Window, QColor("#EAF2FF"));
    palette.setColor(QPalette::WindowText, QColor("#0B2A6B"));
    palette.setColor(QPalette::Base, QColor("#FFFFFF"));
    palette.setColor(QPalette::AlternateBase, QColor("#F8FAFD"));
    palette.setColor(QPalette::Highlight, QColor("#1E6FFF"));
    palette.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
    app.setPalette(palette);

    QFile styleFile(":/theme.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }
}

static std::string prepareUserDir(MoneyManager& mm, const std::string& dataDir) {
    std::string userDir = MoneyManager::getUserDataDir(dataDir, mm.getCurrentUsername());
    fs::create_directories(userDir);
    if (fs::exists(userDir + "/wallets.dat")) {
        mm.loadAll(userDir);
    }
    return userDir;
}

static void captureScreenshots(QApplication& app, MoneyManager& mm, const std::string& dataDir, const QString& screenshotTarget) {
    if (!mm.login("admin", "123456")) {
        mm.login("thang", "552007");
    }
    std::string userDir = prepareUserDir(mm, dataDir);
    QString subDir = (screenshotTarget == "125") ? "dpi125" : "dpi100";
    QString outDir = "D:/Downloads/WORKSPACE/PBL2/build/screenshots/" + subDir;
    QDir().mkpath(outDir);
    QDir().mkpath("D:/Downloads/WORKSPACE/PBL2/build/screenshots");

    QFile logFile(outDir + "/log.txt");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logFile.write(QString("Screenshot mode %1 starting...\n").arg(screenshotTarget).toUtf8());
        logFile.close();
    }

    MainWindow mainWin(mm, userDir, dataDir);
    mainWin.resize(1280, 800);
    mainWin.show();
    app.processEvents();

    // 1-5. 5 tab chính
    int pageIndices[] = {0, 1, 2, 3, 4};
    const char* pageFiles[] = {"/01_dashboard.png", "/02_transactions.png", "/03_wallets.png", "/04_statistics.png", "/05_settings.png"};
    for (int i = 0; i < 5; ++i) {
        if (i == 2) UiHelper::balanceHidden = false;
        mainWin.navigateToPage(pageIndices[i]);
        app.processEvents();
        mainWin.grab().save(outDir + pageFiles[i]);
        if (screenshotTarget == "100") mainWin.grab().save(QString("D:/Downloads/WORKSPACE/PBL2/build/screenshots") + pageFiles[i]);
    }

    // 6-7. Popup chọn ngày
    DatePickerPopup myPicker(QDate(2026, 11, 10), &mainWin);
    myPicker.resize(380, 480);
    myPicker.show();
    app.processEvents();
    myPicker.grab().save(outDir + "/06_month_year_picker.png");
    myPicker.grab().save(outDir + "/07_date_picker_popup.png");
    myPicker.close();

    // 8. Hộp thoại chi tiết ngày
    DayDetailDialog ddDialog(mm, Date(10, 11, 2026), &mainWin, &mainWin);
    ddDialog.resize(780, 540);
    ddDialog.show();
    app.processEvents();
    ddDialog.grab().save(outDir + "/08_day_detail_dialog.png");
    ddDialog.close();

    // 9. Hộp thoại Thêm/Sửa Giao dịch
    TransactionDialog txDialog(mm, -1, &mainWin);
    txDialog.resize(560, 680);
    txDialog.show();
    app.processEvents();
    txDialog.grab().save(outDir + "/09_transaction_dialog.png");
    txDialog.close();

    // 10. Hộp thoại Thêm/Sửa Ví
    WalletDialog wDialog(mm, -1, &mainWin);
    wDialog.resize(480, 420);
    wDialog.show();
    app.processEvents();
    wDialog.grab().save(outDir + "/10_wallet_dialog.png");
    wDialog.close();

    // 11. Hộp thoại Thiết lập kế hoạch
    BudgetPlan plan;
    PlanDialog::loadPlanFromFile(userDir, plan);
    PlanDialog pDialog(plan, &mainWin);
    pDialog.resize(480, 500);
    pDialog.show();
    app.processEvents();
    pDialog.grab().save(outDir + "/11_plan_dialog.png");
    pDialog.close();

    // 12-13. Đăng nhập & Đăng ký
    LoginWindow lWin(mm, dataDir);
    lWin.resize(800, 500);
    lWin.show();
    app.processEvents();
    lWin.grab().save(outDir + "/12_login_window.png");
    lWin.close();

    RegisterDialog rDialog(mm, dataDir, &mainWin);
    rDialog.resize(420, 480);
    rDialog.show();
    app.processEvents();
    rDialog.grab().save(outDir + "/13_register_dialog.png");
    rDialog.close();

    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        logFile.write("All screenshots saved successfully!\n");
        logFile.close();
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    initStyle(app);
    std::string dataDir = DATA_DIR;
    MoneyManager mm;
    mm.loadUsers(dataDir);

    QString screenshotTarget = "";
    for (const QString& arg : app.arguments()) {
        if (arg == "--screenshot" || arg == "--screenshot=100") {
            screenshotTarget = "100";
            break;
        } else if (arg == "--screenshot=125") {
            screenshotTarget = "125";
            break;
        }
    }

    if (!screenshotTarget.isEmpty()) {
        captureScreenshots(app, mm, dataDir, screenshotTarget);
        return 0;
    }

    while (true) {
        LoginWindow loginWin(mm, dataDir);
        if (loginWin.exec() != QDialog::Accepted) break;
        MainWindow mainWin(mm, prepareUserDir(mm, dataDir), dataDir);
        mainWin.show();
        app.exec();
        if (!mainWin.isLoggedOut()) break;
    }
    return 0;
}
