#include "gui_helpers.h"
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QGraphicsDropShadowEffect>
#include "../core/utils/utils.h"
#include "../core/manager/transaction_manager.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#ifndef DWMWA_CAPTION_COLOR
#define DWMWA_CAPTION_COLOR 35
#endif
#ifndef DWMWA_TEXT_COLOR
#define DWMWA_TEXT_COLOR 36
#endif
#endif

namespace UiHelper {

bool balanceHidden = true;

QString formatBalance(long long amount) {
    if (balanceHidden) {
        return QString(8, QChar(0x2022));
    }
    return formatMoneyVND(amount);
}

QString cleanName(const QString& input) {
    QString trimmed = input.trimmed();
    static QRegularExpression re("\\s+");
    return trimmed.replace(re, " ");
}

bool isValidName(const QString& name) {
    if (name.isEmpty()) return false;
    if (name.contains('|') || name.contains('\n') || name.contains('\r')) return false;
    return true;
}

QString formatMoneyVND(long long amount) {
    std::string s = formatMoney(amount);
    return QString::fromStdString(s) + " đ";
}

QString formatDate(const Date& date) {
    return QString::fromStdString(date.toString());
}

static const char* kMsgBoxStyle =
    "QMessageBox { background-color: #FFFFFF; border: 1px solid #DCE8FA; border-radius: 14px; } "
    "QLabel { color: #0B2A6B; font-size: 14px; min-width: 260px; } "
    "QPushButton { background-color: #1E6FFF; color: #FFFFFF; border: none; "
    "border-radius: 10px; font-weight: 600; min-height: 38px; min-width: 80px; padding: 0 16px; } "
    "QPushButton:hover { background-color: #155EEF; }";

void showError(QWidget* parent, const QString& message) {
    QMessageBox box(parent);
    box.setWindowTitle("Thông báo lỗi");
    box.setText(message);
    box.setIcon(QMessageBox::Warning);
    box.setStyleSheet(kMsgBoxStyle);
    box.exec();
}

void showInfo(QWidget* parent, const QString& message) {
    QMessageBox box(parent);
    box.setWindowTitle("Thông báo");
    box.setText(message);
    box.setIcon(QMessageBox::Information);
    box.setStyleSheet(kMsgBoxStyle);
    box.exec();
}

bool askConfirmation(QWidget* parent, const QString& title, const QString& message) {
    QMessageBox box(parent);
    box.setWindowTitle(title);
    box.setText(message);
    box.setIcon(QMessageBox::Question);
    QPushButton* btnYes = box.addButton("Đồng ý", QMessageBox::YesRole);
    QPushButton* btnNo = box.addButton("Hủy", QMessageBox::NoRole);
    box.setDefaultButton(btnNo);
    box.setStyleSheet(
        "QMessageBox { background-color: #FFFFFF; border: 1px solid #DCE8FA; border-radius: 14px; } "
        "QLabel { color: #0B2A6B; font-size: 14px; min-width: 280px; } "
        "QPushButton[role=\"yes\"] { background-color: #1E6FFF; color: #FFFFFF; border: none; "
        "border-radius: 10px; font-weight: 600; min-height: 38px; min-width: 80px; padding: 0 16px; } "
        "QPushButton[role=\"yes\"]:hover { background-color: #155EEF; } "
        "QPushButton[role=\"no\"] { background-color: #FFFFFF; color: #5B6B8C; border: 1.5px solid #D5E1F5; "
        "border-radius: 10px; font-weight: 600; min-height: 38px; min-width: 80px; padding: 0 16px; } "
        "QPushButton[role=\"no\"]:hover { background-color: #F8FAFD; border-color: #CBD5E1; }"
    );
    btnYes->setProperty("role", "yes");
    btnNo->setProperty("role", "no");
    box.exec();
    return box.clickedButton() == btnYes;
}

void setupWindowTheme(QWidget* widget) {
#ifdef Q_OS_WIN
    if (!widget) return;
    HWND hwnd = reinterpret_cast<HWND>(widget->winId());
    if (!hwnd) return;

    HMODULE dwmapi = GetModuleHandleW(L"dwmapi.dll");
    if (!dwmapi) {
        dwmapi = LoadLibraryW(L"dwmapi.dll");
    }
    if (dwmapi) {
        typedef HRESULT (WINAPI *DwmSetWindowAttrFunc)(HWND, DWORD, LPCVOID, DWORD);
        DwmSetWindowAttrFunc pDwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttrFunc>(
            GetProcAddress(dwmapi, "DwmSetWindowAttribute")
        );
        if (pDwmSetWindowAttribute) {
            COLORREF captionColor = RGB(30, 111, 255); // #1E6FFF
            COLORREF textColor = RGB(255, 255, 255);
            pDwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &captionColor, sizeof(captionColor));
            pDwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR, &textColor, sizeof(textColor));
        }
    }
#else
    Q_UNUSED(widget);
#endif
}

void showBalanceError(QWidget* parent, const BalanceValidationResult& res) {
    QString title = "Không thể thực hiện giao dịch";
    if (res.errorCode == ValidationErrorCode::INSUFFICIENT_FUNDS) {
        title = "Số dư không đủ";
    } else if (res.errorCode == ValidationErrorCode::WALLET_ALREADY_NEGATIVE) {
        title = "Ví đang có số dư âm";
    } else if (res.errorCode == ValidationErrorCode::WOULD_GO_NEGATIVE_ON_DATE) {
        title = "Số dư sẽ bị âm trong lịch sử";
    }
    QMessageBox box(QMessageBox::Warning, title,
                    QString::fromStdString(res.errorMessage), QMessageBox::Ok, parent);
    box.exec();
}

void applyCardShadow(QWidget* target, int blurRadius, int offsetY, qreal opacity) {
    if (!target) return;
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(target);
    shadow->setBlurRadius(blurRadius);
    shadow->setOffset(0, offsetY);
    shadow->setColor(QColor(11, 42, 107, static_cast<int>(opacity * 255)));
    target->setGraphicsEffect(shadow);
}

void applyCardShadows(const std::initializer_list<QWidget*>& targets, int blurRadius, int offsetY, qreal opacity) {
    for (QWidget* w : targets) {
        applyCardShadow(w, blurRadius, offsetY, opacity);
    }
}

QColor colorPrimary()   { return QColor("#1E6FFF"); }
QColor colorNavy()      { return QColor("#0B2A6B"); }
QColor colorIncome()    { return QColor("#0E8F4F"); }
QColor colorExpense()   { return QColor("#E0173D"); }
QColor colorIncomeBg()  { return QColor("#EAFBF3"); }
QColor colorExpenseBg() { return QColor("#FFF0F3"); }

QString getCategoryIcon(const QString& name) {
    QString lower = name.toLower();
    if (lower.contains("lương") || lower.contains("thưởng") || lower.contains("thu nhập"))
        return QString::fromUtf8("💰");
    if (lower.contains("ăn") || lower.contains("cơm") || lower.contains("uống") || lower.contains("cà phê"))
        return QString::fromUtf8("🍜");
    if (lower.contains("mua") || lower.contains("sắm") || lower.contains("quần") || lower.contains("áo"))
        return QString::fromUtf8("🛍️");
    if (lower.contains("trọ") || lower.contains("nhà") || lower.contains("điện") || lower.contains("nước"))
        return QString::fromUtf8("🏠");
    if (lower.contains("xe") || lower.contains("xăng") || lower.contains("đi lại"))
        return QString::fromUtf8("🚗");
    if (lower.contains("học") || lower.contains("sách"))
        return QString::fromUtf8("📚");
    return QString::fromUtf8("🏷️");
}

QString getWalletIcon(const QString& name, int walletType) {
    QString lower = name.toLower();
    if (lower.contains("tiền mặt") || walletType == 0)
        return QString::fromUtf8("💵");
    if (lower.contains("momo") || lower.contains("zalo") || lower.contains("viettel") || walletType == 2)
        return QString::fromUtf8("📱");
    return QString::fromUtf8("🏦");
}

} // namespace UiHelper
