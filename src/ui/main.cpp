#include <QApplication>
#include <QFile>
#include <QFont>
#include <QPalette>
#include <filesystem>
#include "../core/app/money_manager.h"
#include "login_window.h"
#include "main_window.h"
#include "dialogs/day_detail_dialog.h"
#include "dialogs/transaction_dialog.h"
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

    // Chuan hoa bang mau nen va mau nhan dong bo he thong (#1E6FFF, #0F172A)
    QPalette palette = app.palette();
    palette.setColor(QPalette::Window, QColor("#F0F5FD"));
    palette.setColor(QPalette::WindowText, QColor("#0F172A"));
    palette.setColor(QPalette::Base, QColor("#FFFFFF"));
    palette.setColor(QPalette::AlternateBase, QColor("#F8FAFC"));
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

        // 1. Trang Tổng quan (Dashboard có hàng nút thao tác nhanh mới)
        mainWin.navigateToPage(0);
        app.processEvents();
        mainWin.grab().save(outDir + "/01_dashboard.png");
        if (screenshotTarget == "100") mainWin.grab().save("D:/Downloads/WORKSPACE/PBL2/build/screenshots/01_dashboard.png");

        // 2. Trang Giao dịch (Transactions có nút có icon & bộ lọc Từ ngày/Đến ngày)
        mainWin.navigateToPage(1);
        app.processEvents();
        mainWin.grab().save(outDir + "/02_transactions.png");
        if (screenshotTarget == "100") mainWin.grab().save("D:/Downloads/WORKSPACE/PBL2/build/screenshots/02_transactions.png");

        // 3. Trang Ví tiền (Wallets có cảnh báo đối soát rõ ràng)
        mainWin.navigateToPage(2);
        app.processEvents();
        mainWin.grab().save(outDir + "/03_wallets.png");
        if (screenshotTarget == "100") mainWin.grab().save("D:/Downloads/WORKSPACE/PBL2/build/screenshots/03_wallets.png");

        // 4. Trang Thống kê (Statistics)
        mainWin.navigateToPage(3);
        app.processEvents();
        mainWin.grab().save(outDir + "/04_statistics.png");
        if (screenshotTarget == "100") mainWin.grab().save("D:/Downloads/WORKSPACE/PBL2/build/screenshots/04_statistics.png");

        // 5. Trang Cài đặt (Settings)
        mainWin.navigateToPage(4);
        app.processEvents();
        mainWin.grab().save(outDir + "/05_settings.png");
        if (screenshotTarget == "100") mainWin.grab().save("D:/Downloads/WORKSPACE/PBL2/build/screenshots/05_settings.png");

        // 6. Bảng lịch chọn ngày chuẩn (DatePickerPopup hiển thị ngày 10/11/2026)
        DatePickerPopup myPicker(QDate(2026, 11, 10), &mainWin);
        myPicker.resize(380, 480);
        myPicker.show();
        app.processEvents();
        myPicker.grab().save(outDir + "/06_month_year_picker.png");
        if (screenshotTarget == "100") myPicker.grab().save("D:/Downloads/WORKSPACE/PBL2/build/screenshots/06_month_year_picker.png");
        myPicker.close();

        // 7. Popup Lịch chọn ngày nâng cao (DatePickerPopup)
        DatePickerPopup dpPopup(QDate(2026, 11, 10), &mainWin);
        dpPopup.resize(380, 480);
        dpPopup.show();
        app.processEvents();
        dpPopup.grab().save(outDir + "/07_date_picker_popup.png");
        if (screenshotTarget == "100") dpPopup.grab().save("D:/Downloads/WORKSPACE/PBL2/build/screenshots/07_date_picker_popup.png");
        dpPopup.close();

        // 8. Hộp thoại chi tiết ngày (DayDetailDialog)
        DayDetailDialog ddDialog(mm, Date(10, 11, 2026), &mainWin, &mainWin);
        ddDialog.resize(780, 540);
        ddDialog.show();
        app.processEvents();
        ddDialog.grab().save(outDir + "/08_day_detail_dialog.png");
        if (screenshotTarget == "100") ddDialog.grab().save("D:/Downloads/WORKSPACE/PBL2/build/screenshots/08_day_detail_dialog.png");
        ddDialog.close();

        // 9. Hộp thoại Thêm/Sửa Giao dịch (TransactionDialog)
        TransactionDialog txDialog(mm, -1, &mainWin);
        txDialog.resize(520, 680);
        txDialog.show();
        app.processEvents();
        txDialog.grab().save(outDir + "/09_transaction_dialog.png");
        if (screenshotTarget == "100") txDialog.grab().save("D:/Downloads/WORKSPACE/PBL2/build/screenshots/09_transaction_dialog.png");
        txDialog.close();

        if (logFile.open(QIODevice::Append | QIODevice::Text)) {
            logFile.write("All screenshots saved successfully!\n");
            logFile.close();
        }
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
