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
#include <QScrollArea>
#include <QScrollBar>
#include <QDir>
#include <QMessageBox>

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
        if (!mm.login("thang", "552007")) {
            mm.login("demo", "123456");
        }
    }
    std::string userDir = prepareUserDir(mm, dataDir);

    QString baseDir = "build/screenshots";
    for (const QString& arg : app.arguments()) {
        if (arg.startsWith("--screenshot-dir=")) {
            baseDir = arg.mid(17);
        }
    }

    QString subDir = (screenshotTarget == "125") ? "dpi125" : "dpi100";
    QString outDir = baseDir + "/" + subDir;
    QDir().mkpath(outDir);
    QDir().mkpath(baseDir);

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
        if (screenshotTarget == "100") mainWin.grab().save(baseDir + pageFiles[i]);
    }

    // 4b. Chụp thêm tab Thống kê cuộn xuống (thấy trọn vẹn card donut và chi tiết danh mục)
    mainWin.navigateToPage(3);
    QScrollArea* sa = mainWin.findChild<QScrollArea*>("scrollStatistics");
    if (sa) {
        sa->verticalScrollBar()->setValue(sa->verticalScrollBar()->maximum());
        app.processEvents();
        mainWin.grab().save(outDir + "/04_statistics_scrolled.png");
        if (screenshotTarget == "100") mainWin.grab().save(baseDir + "/04_statistics_scrolled.png");
        mainWin.resize(1100, 700);
        app.processEvents();
        mainWin.grab().save(outDir + "/04_statistics_1100x700.png");
        if (screenshotTarget == "100") mainWin.grab().save(baseDir + "/04_statistics_1100x700.png");
        mainWin.showMaximized();
        app.processEvents();
        mainWin.grab().save(outDir + "/04_statistics_maximized.png");
        if (screenshotTarget == "100") mainWin.grab().save(baseDir + "/04_statistics_maximized.png");
        mainWin.showNormal();
        mainWin.resize(1280, 800);
        sa->verticalScrollBar()->setValue(0);
        app.processEvents();
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

    // 9. Hộp thoại Thêm Giao dịch
    TransactionDialog txDialog(mm, -1, &mainWin);
    txDialog.show();
    app.processEvents();
    txDialog.grab().save(outDir + "/09_transaction_dialog.png");
    if (screenshotTarget == "100") txDialog.grab().save(baseDir + "/09_transaction_dialog.png");
    txDialog.close();

    // 9b. Hộp thoại Sửa Giao dịch
    const auto& txs = mm.getTransactionManager().getTransactions();
    int firstTxId = txs.getHead() ? txs.getHead()->data->getId() : -1;
    if (firstTxId > 0) {
        TransactionDialog txEditDialog(mm, firstTxId, &mainWin);
        txEditDialog.show();
        app.processEvents();
        txEditDialog.grab().save(outDir + "/09_transaction_dialog_edit.png");
        if (screenshotTarget == "100") txEditDialog.grab().save(baseDir + "/09_transaction_dialog_edit.png");
        txEditDialog.close();
    }

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

    // 14. Hop thoai loi chan so du
    QMessageBox errBox(&mainWin);
    errBox.setWindowTitle(QString::fromUtf8("Lỗi"));
    errBox.setText(QString::fromUtf8("Không thể thêm giao dịch (số dư ví Tiền mặt không đủ hoặc làm âm số dư trong tương lai)."));
    errBox.setIcon(QMessageBox::Warning);
    errBox.show();
    app.processEvents();
    errBox.grab().save(outDir + "/14_error_balance_dialog.png");
    if (screenshotTarget == "100") errBox.grab().save(baseDir + "/14_error_balance_dialog.png");
    errBox.close();

    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        logFile.write("All screenshots saved successfully!\n");
        logFile.close();
    }
}

static bool handleScreenshotOption(QApplication& app, MoneyManager& mm, const std::string& dataDir) {
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
    if (screenshotTarget.isEmpty()) {
        return false;
    }
    captureScreenshots(app, mm, dataDir, screenshotTarget);
    return true;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    initStyle(app);
    std::string dataDir = DATA_DIR;
    for (const QString& arg : app.arguments()) {
        if (arg.startsWith("--data-dir=")) {
            dataDir = arg.mid(11).toStdString();
        }
    }
    MoneyManager mm;
    mm.loadUsers(dataDir);

    if (handleScreenshotOption(app, mm, dataDir)) {
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
