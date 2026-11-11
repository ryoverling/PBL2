#include "gui_helpers.h"
#include <QMessageBox>
#include <QPushButton>
#include <QAbstractButton>
#include <QRegularExpression>
#include <QIcon>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include "../core/utils/utils.h"

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

    // Boc an toan bang cach kiem tra runtime dwmapi.dll (tuong thich Windows 10 & 11)
    HMODULE dwmapi = GetModuleHandleW(L"dwmapi.dll");
    if (!dwmapi) {
        dwmapi = LoadLibraryW(L"dwmapi.dll");
    }
    if (dwmapi) {
        typedef HRESULT (WINAPI *DwmSetWindowAttrFunc)(HWND, DWORD, LPCVOID, DWORD);
        auto pDwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttrFunc>(
            GetProcAddress(dwmapi, "DwmSetWindowAttribute")
        );
        if (pDwmSetWindowAttribute) {
            // DWMWA_CAPTION_COLOR (35) va DWMWA_TEXT_COLOR (36) chi hoat dong tren Win 11 (Build 22000+)
            // Tren Windows 10, Windows chi tra ve E_INVALIDARG ma khong crash hay anh huong ung dung
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

void applyCardShadow(QWidget* target, int blurRadius, int offsetY, qreal opacity) {
    if (!target) return;
    auto* shadow = new QGraphicsDropShadowEffect(target);
    shadow->setBlurRadius(blurRadius);
    shadow->setOffset(0, offsetY);
    shadow->setColor(QColor(11, 42, 107, static_cast<int>(opacity * 255)));
    target->setGraphicsEffect(shadow);
}

QColor colorPrimary() { return QColor("#1E6FFF"); }
QColor colorNavy()    { return QColor("#0B2A6B"); }
QColor colorIncome()  { return QColor("#0E8F4F"); }
QColor colorExpense() { return QColor("#E0173D"); }

QIcon createAppLogoIcon(int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    // Nen gradient xanh bo tron
    QLinearGradient bg(0, 0, size, size);
    bg.setColorAt(0.0, QColor("#1E6FFF"));
    bg.setColorAt(1.0, QColor("#0B2A6B"));
    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    p.drawRoundedRect(0, 0, size, size, 8, 8);

    // 3 cot bieu do mau trang
    p.setBrush(Qt::white);
    int barW = size / 6;
    int r = barW / 2;
    int gap = size / 8;
    int x1 = size / 5;
    int x2 = x1 + barW + gap;
    int x3 = x2 + barW + gap;
    int b = size - size / 5;

    p.drawRoundedRect(x1, b - size / 3, barW, size / 3, r, r);
    p.drawRoundedRect(x2, b - size / 2, barW, size / 2, r, r);
    p.drawRoundedRect(x3, b - (size * 2) / 3, barW, (size * 2) / 3, r, r);
    return QIcon(pix);
}

QIcon createHomeNavIcon(const QColor& color) {
    QPixmap pix(24, 24);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // Mai nha
    QPointF roof[3] = { QPointF(12, 4), QPointF(4, 11), QPointF(20, 11) };
    p.drawPolyline(roof, 3);
    // Than nha
    p.drawRect(6, 11, 12, 9);
    // Cua nha nho
    p.drawLine(12, 16, 12, 20);
    return QIcon(pix);
}

QIcon createExchangeNavIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // Mui ten tren huong sang phai
    p.drawLine(4, 7, 16, 7);
    p.drawLine(12, 4, 16, 7);
    // Mui ten duoi huong sang trai
    p.drawLine(16, 13, 4, 13);
    p.drawLine(8, 16, 4, 13);
    return QIcon(pix);
}

QIcon createWalletNavIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawRoundedRect(3, 5, 14, 10, 2.5, 2.5);
    p.drawPoint(13, 10);
    return QIcon(pix);
}

QIcon createStatsNavIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(5, 16, 5, 10);
    p.drawLine(10, 16, 10, 4);
    p.drawLine(15, 16, 15, 7);
    return QIcon(pix);
}

QIcon createSettingsNavIcon(const QColor& color) {
    QPixmap pix(24, 24);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap));
    // Vong tron trong
    p.drawEllipse(8, 8, 8, 8);
    // 8 rang banh rang co khi
    p.drawLine(12, 2, 12, 5);
    p.drawLine(12, 19, 12, 22);
    p.drawLine(2, 12, 5, 12);
    p.drawLine(19, 12, 22, 12);
    p.drawLine(5, 5, 7, 7);
    p.drawLine(17, 17, 19, 19);
    p.drawLine(19, 5, 17, 7);
    p.drawLine(5, 19, 7, 17);
    return QIcon(pix);
}

QIcon createUserIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawEllipse(7, 3, 6, 6);
    QPainterPath path;
    path.arcMoveTo(3, 8, 14, 10, 0);
    path.arcTo(3, 8, 14, 10, 0, 180);
    p.drawPath(path);
    return QIcon(pix);
}

QIcon createLockIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawRoundedRect(4, 8, 12, 9, 2, 2);
    p.drawArc(6, 3, 8, 8, 0 * 16, 180 * 16);
    p.drawPoint(10, 12);
    return QIcon(pix);
}

QIcon createRightArrowIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawLine(5, 10, 14, 10);
    p.drawLine(11, 6, 15, 10);
    p.drawLine(11, 14, 15, 10);
    return QIcon(pix);
}

QIcon createKeyIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap));
    p.drawEllipse(4, 5, 6, 6);
    p.drawLine(10, 10, 16, 15);
    p.drawLine(14, 13, 16, 11);
    return QIcon(pix);
}

QPixmap createLoginIllustration(int width, int height) {
    QPixmap pix(width, height);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    // Hinh minh hoa vi tien xanh 3D + xu vang trang tri
    QLinearGradient grad(0, 0, width, height);
    grad.setColorAt(0.0, QColor("#1E6FFF"));
    grad.setColorAt(1.0, QColor("#0B2A6B"));
    p.setBrush(grad);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(width / 4, height / 3, width / 2, height / 2, 12, 12);

    // Nap vi
    p.setBrush(QColor("#3B82F6"));
    p.drawRoundedRect(width / 4 + 4, height / 3 + 4, width / 2 - 8, height / 5, 8, 8);

    // Khoa vi
    p.setBrush(QColor("#F59E0B"));
    p.drawEllipse(width / 2 - 6, height / 2 - 2, 12, 12);

    // Tien xu vang ben canh
    p.setBrush(QColor("#F59E0B"));
    p.drawEllipse(width / 5, height / 2 + 10, 20, 20);
    p.setBrush(QColor("#FBBF24"));
    p.drawEllipse(width / 5 + 3, height / 2 + 13, 14, 14);

    return pix;
}

QIcon createPlusIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 2.2, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(10, 4, 10, 16);
    p.drawLine(4, 10, 16, 10);
    return QIcon(pix);
}

QIcon createEditIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawLine(4, 16, 8, 16);
    p.drawLine(4, 16, 5, 12);
    p.drawLine(5, 12, 13, 4);
    p.drawLine(13, 4, 16, 7);
    p.drawLine(16, 7, 8, 15);
    p.drawLine(8, 15, 4, 16);
    return QIcon(pix);
}

QIcon createTrashIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // Nắp thùng
    p.drawLine(4, 6, 16, 6);
    p.drawLine(8, 6, 8, 4);
    p.drawLine(8, 4, 12, 4);
    p.drawLine(12, 4, 12, 6);
    // Thân thùng
    p.drawLine(6, 6, 7, 16);
    p.drawLine(7, 16, 13, 16);
    p.drawLine(13, 16, 14, 6);
    // Nan thùng
    p.drawLine(9, 9, 9, 13);
    p.drawLine(11, 9, 11, 13);
    return QIcon(pix);
}

QIcon createListIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(4, 6, 5, 6);
    p.drawLine(8, 6, 16, 6);
    p.drawLine(4, 10, 5, 10);
    p.drawLine(8, 10, 16, 10);
    p.drawLine(4, 14, 5, 14);
    p.drawLine(8, 14, 16, 14);
    return QIcon(pix);
}

QIcon createCalendarIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawRoundedRect(3, 4, 14, 13, 3, 3);
    p.drawLine(3, 8, 17, 8);
    p.drawLine(6, 2, 6, 5);
    p.drawLine(14, 2, 14, 5);
    return QIcon(pix);
}

QIcon createSearchIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawEllipse(4, 4, 8, 8);
    p.drawLine(10, 10, 15, 15);
    return QIcon(pix);
}

QIcon createFilterIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPointF pts[5] = { QPointF(4, 5), QPointF(16, 5), QPointF(11, 11), QPointF(11, 15), QPointF(9, 13) };
    p.drawPolyline(pts, 5);
    p.drawLine(9, 13, 9, 11);
    p.drawLine(9, 11, 4, 5);
    return QIcon(pix);
}

QIcon createWalletIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawRoundedRect(3, 5, 14, 10, 2, 2);
    p.drawPoint(13, 10);
    return QIcon(pix);
}

QIcon createTagIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPointF pts[5] = { QPointF(4, 10), QPointF(10, 4), QPointF(16, 4), QPointF(16, 10), QPointF(10, 16) };
    p.drawPolygon(pts, 5);
    p.drawPoint(13, 7);
    return QIcon(pix);
}

QIcon createArrowUpIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawLine(10, 15, 10, 5);
    QPointF pts[3] = { QPointF(6, 9), QPointF(10, 5), QPointF(14, 9) };
    p.drawPolyline(pts, 3);
    return QIcon(pix);
}

QIcon createArrowDownIcon(const QColor& color) {
    QPixmap pix(20, 20);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawLine(10, 5, 10, 15);
    QPointF pts[3] = { QPointF(6, 11), QPointF(10, 15), QPointF(14, 11) };
    p.drawPolyline(pts, 3);
    return QIcon(pix);
}

QString getCategoryIcon(const QString& name) {
    QString lower = name.toLower().trimmed();
    if (lower.contains("ăn") || lower.contains("uống") || lower.contains("thực phẩm") || lower.contains("cà phê")) return QString::fromUtf8("🍜");
    if (lower.contains("đi lại") || lower.contains("xăng") || lower.contains("xe") || lower.contains("grab")) return QString::fromUtf8("🚗");
    if (lower.contains("trọ") || lower.contains("nhà") || lower.contains("thuê")) return QString::fromUtf8("🏠");
    if (lower.contains("mua sắm") || lower.contains("quần áo") || lower.contains("shopping")) return QString::fromUtf8("🛍️");
    if (lower.contains("lương") || lower.contains("thu nhập")) return QString::fromUtf8("💰");
    if (lower.contains("thưởng") || lower.contains("quà")) return QString::fromUtf8("🎁");
    if (lower.contains("học") || lower.contains("sách") || lower.contains("giáo dục")) return QString::fromUtf8("📚");
    if (lower.contains("thuốc") || lower.contains("y tế") || lower.contains("bệnh") || lower.contains("khám")) return QString::fromUtf8("💊");
    if (lower.contains("giải trí") || lower.contains("phim") || lower.contains("game")) return QString::fromUtf8("🎮");
    if (lower.contains("điện") || lower.contains("nước") || lower.contains("mạng") || lower.contains("hóa đơn") || lower.contains("dịch vụ")) return QString::fromUtf8("⚡");
    if (lower.contains("đầu tư") || lower.contains("lãi")) return QString::fromUtf8("📈");
    return QString::fromUtf8("🏷️");
}

QString getWalletIcon(const QString& name, int walletType) {
    QString lower = name.toLower().trimmed();
    if (lower.contains("tiền mặt") || walletType == 0) return QString::fromUtf8("💵");
    if (lower.contains("momo") || lower.contains("zalo") || lower.contains("viettel") || lower.contains("ví") || walletType == 2) return QString::fromUtf8("📱");
    return QString::fromUtf8("🏦");
}

QPixmap createSparkline(bool up, int width, int height) {
    QPixmap pix(width, height);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    QColor color = up ? QColor("#0E8F4F") : QColor("#E0173D");
    color.setAlpha(190);
    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPainterPath path;
    if (up) {
        path.moveTo(4, height - 7);
        path.cubicTo(width * 0.3, height - 5, width * 0.4, height * 0.3, width * 0.7, height * 0.45);
        path.cubicTo(width * 0.85, height * 0.55, width * 0.9, height * 0.2, width - 4, 4);
    } else {
        path.moveTo(4, height * 0.45);
        path.cubicTo(width * 0.25, height * 0.65, width * 0.5, height * 0.2, width * 0.75, height * 0.7);
        path.cubicTo(width * 0.85, height * 0.9, width * 0.92, height * 0.35, width - 4, height * 0.5);
    }
    p.drawPath(path);
    return pix;
}

QIcon createCategoryCircleIcon(const QString& categoryName, int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QString nameLower = categoryName.toLower();
    QColor bgColor("#F1F5F9");
    QColor borderColor("#E2E8F0");
    QString iconChar = QString::fromUtf8("🏷️");

    if (nameLower.contains("lương") || nameLower.contains("thưởng") || nameLower.contains("thu nhập")) {
        bgColor = QColor("#FEF3C7");
        borderColor = QColor("#FDE68A");
        iconChar = QString::fromUtf8("💰");
    } else if (nameLower.contains("ăn") || nameLower.contains("cơm") || nameLower.contains("cà phê")) {
        bgColor = QColor("#FEE2E2");
        borderColor = QColor("#FECACA");
        iconChar = QString::fromUtf8("🍜");
    } else if (nameLower.contains("iphone") || nameLower.contains("mua sắm") || nameLower.contains("quần áo")) {
        bgColor = QColor("#FFEDD5");
        borderColor = QColor("#FED7AA");
        iconChar = QString::fromUtf8("🛍️");
    } else if (nameLower.contains("trọ") || nameLower.contains("điện") || nameLower.contains("nhà")) {
        bgColor = QColor("#E0EDFF");
        borderColor = QColor("#BFDBFE");
        iconChar = QString::fromUtf8("🏠");
    } else if (nameLower.contains("đi lại") || nameLower.contains("xăng") || nameLower.contains("xe")) {
        bgColor = QColor("#CCFBF1");
        borderColor = QColor("#99F6E4");
        iconChar = QString::fromUtf8("🚗");
    }

    p.setBrush(bgColor);
    p.setPen(QPen(borderColor, 1));
    p.drawEllipse(1, 1, size - 2, size - 2);

    QFont font = p.font();
    font.setPixelSize(size * 0.5);
    p.setFont(font);
    p.drawText(QRect(0, 0, size, size), Qt::AlignCenter, iconChar);
    return QIcon(pix);
}

QPixmap createPlanBadge(const QString& type, int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    if (type == "budget") {
        p.setBrush(QColor("#FEE2E2"));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(QRect(1, 1, size - 2, size - 2), 6, 6);
        p.setBrush(QColor("#E0173D"));
        p.drawRoundedRect(4, size - 9, 3, 5, 1, 1);
        p.drawRoundedRect(9, size - 15, 3, 11, 1, 1);
        p.drawRoundedRect(14, size - 12, 3, 8, 1, 1);
    } else {
        p.setBrush(QColor("#E0EDFF"));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(QRect(1, 1, size - 2, size - 2), 6, 6);
        p.setPen(QPen(QColor("#1E6FFF"), 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        p.drawLine(4, 7, 7, 7);
        p.drawLine(7, 7, 9, 15);
        p.drawLine(9, 15, 17, 15);
        p.drawLine(17, 15, 19, 9);
        p.drawLine(19, 9, 7, 9);
    }
    return pix;
}

}
