#include "gui_helpers.h"
#include <QMessageBox>
#include <QAbstractButton>
#include <QRegularExpression>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include "../core/utils/format_utils.h"

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

void showError(QWidget* parent, const QString& message) {
    QMessageBox::warning(parent, "Thông báo lỗi", message);
}

void showInfo(QWidget* parent, const QString& message) {
    QMessageBox::information(parent, "Thông báo", message);
}

bool askConfirmation(QWidget* parent, const QString& title, const QString& message) {
    QMessageBox box(QMessageBox::Question, title, message, QMessageBox::Yes | QMessageBox::No, parent);
    box.button(QMessageBox::Yes)->setProperty("role", "primary");
    box.button(QMessageBox::No)->setProperty("role", "secondary");
    box.setDefaultButton(QMessageBox::No);
    return box.exec() == QMessageBox::Yes;
}

void setupWindowTheme(QWidget* widget) {
#ifdef Q_OS_WIN
    if (!widget) return;
    HWND hwnd = reinterpret_cast<HWND>(widget->winId());
    if (hwnd) {
        COLORREF captionColor = RGB(23, 105, 238); // #1769EE primary blue
        COLORREF textColor = RGB(255, 255, 255);
        DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &captionColor, sizeof(captionColor));
        DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR, &textColor, sizeof(textColor));
    }
#else
    Q_UNUSED(widget);
#endif
}

QColor colorIncome() {
    return QColor("#10B981");
}

QColor colorExpense() {
    return QColor("#FF405C");
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

}
