#pragma once

#include <QString>
#include <QColor>
#include <QWidget>
#include <QIcon>
#include <QPixmap>
#include "../core/utils/utils.h"

// Cac ham tien ich dung chung cho giao dien Qt (chuan sinh vien nam 2)
namespace UiHelper {
    // Co trang thai an/hien so du
    extern bool balanceHidden;

    // Dinh dang so du (tra ve 8 ky tu • khi an, nguoc lai dinh dang tien te)
    QString formatBalance(long long amount);

    // Dinh dang tien te hien thi (vi du: 1.500.000 d)
    QString formatMoneyVND(long long amount);

    // Dinh dang ngay thang hien thi (DD/MM/YYYY)
    QString formatDate(const Date& date);

    // Hien thi hop thoai thong bao loi (restyle QMessageBox)
    void showError(QWidget* parent, const QString& message);

    // Hien thi hop thoai thong bao thong tin (restyle QMessageBox)
    void showInfo(QWidget* parent, const QString& message);

    // Hop thoai xac nhan voi nut Dong y noi bat va Huy la nut phu
    bool askConfirmation(QWidget* parent, const QString& title, const QString& message);

    // Chuan hoa ten: cat khoang trang dau/cuoi, gop khoang trang thua
    QString cleanName(const QString& input);

    // Kiem tra ten hop le
    bool isValidName(const QString& name);

    // Thiet lap mau thanh tieu de he thong Windows dong bo mau xanh #1E6FFF
    void setupWindowTheme(QWidget* widget);

    // Gan hieu ung do bong mem cho the Card
    void applyCardShadow(QWidget* target, int blurRadius = 14, int offsetY = 2, qreal opacity = 0.08);

    // Bang mau chuan he thong MoneyManager
    QColor colorPrimary();  // #1E6FFF
    QColor colorNavy();     // #0B2A6B
    QColor colorIncome();   // #0E8F4F
    QColor colorExpense();  // #E0173D

    // Icon Vector cho Navigation Header va App Logo
    QIcon createAppLogoIcon(int size = 36);
    QIcon createHomeNavIcon(const QColor& color = QColor("#5B6B8C"));
    QIcon createExchangeNavIcon(const QColor& color = QColor("#5B6B8C"));
    QIcon createWalletNavIcon(const QColor& color = QColor("#5B6B8C"));
    QIcon createStatsNavIcon(const QColor& color = QColor("#5B6B8C"));
    QIcon createSettingsNavIcon(const QColor& color = QColor("#5B6B8C"));

    // Icon Vector cho Form Dang nhap va Thao tac
    QIcon createUserIcon(const QColor& color = QColor("#5B6B8C"));
    QIcon createLockIcon(const QColor& color = QColor("#5B6B8C"));
    QIcon createRightArrowIcon(const QColor& color = Qt::white);
    QIcon createKeyIcon(const QColor& color = QColor("#1E6FFF"));
    QPixmap createLoginIllustration(int width = 200, int height = 120);

    // Icon Vector cho Button thao tac chung
    QIcon createPlusIcon(const QColor& color = Qt::white);
    QIcon createEditIcon(const QColor& color = QColor("#1E6FFF"));
    QIcon createTrashIcon(const QColor& color = QColor("#E0173D"));
    QIcon createListIcon(const QColor& color = QColor("#1E6FFF"));
    QIcon createCalendarIcon(const QColor& color = QColor("#1E6FFF"));
    QIcon createSearchIcon(const QColor& color = QColor("#5B6B8C"));
    QIcon createFilterIcon(const QColor& color = QColor("#5B6B8C"));
    QIcon createWalletIcon(const QColor& color = QColor("#5B6B8C"));
    QIcon createTagIcon(const QColor& color = QColor("#5B6B8C"));
    QIcon createArrowUpIcon(const QColor& color = QColor("#0E8F4F"));
    QIcon createArrowDownIcon(const QColor& color = QColor("#E0173D"));

    // Bieu tuong danh muc va vi tien
    QString getCategoryIcon(const QString& categoryName);
    QString getWalletIcon(const QString& walletName, int walletType = 0);

    // Sparkline cho the KPI Dashboard
    QPixmap createSparkline(bool up, int width = 56, int height = 26);

    // Icon tron pastel cho bang giao dich
    QIcon createCategoryCircleIcon(const QString& categoryName, int size = 28);

    // Icon vuong bo goc cho ke hoach thang nay
    QPixmap createPlanBadge(const QString& type, int size = 24);
}
