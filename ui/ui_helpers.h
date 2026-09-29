#pragma once

#include <QString>
#include <QLocale>
#include <QDate>
#include <QDateTime>

#include "../model/wallet.h"
#include "../model/category.h"

namespace UiHelpers {
    inline QString money(double value) {
        QLocale locale(QLocale::Vietnamese, QLocale::Vietnam);
        return locale.toString(value, 'f', 0) + " đ";
    }

    inline QString walletTypeLabel(WalletType type) {
        switch (type) {
            case WalletType::CASH: return "Tiền mặt";
            case WalletType::BANK: return "Ngân hàng";
            case WalletType::EWALLET: return "Ví điện tử";
            default: return "Khác";
        }
    }

    inline QString categoryTypeLabel(CategoryType type) {
        return type == CategoryType::INCOME ? "Thu nhập" : "Chi tiêu";
    }

    inline QDate parseDate(const std::string& value) {
        return QDate::fromString(QString::fromStdString(value), "yyyy-MM-dd");
    }

    inline QString dateDisplay(const std::string& value) {
        QDate date = parseDate(value);
        return date.isValid() ? date.toString("dd/MM/yyyy") : QString::fromStdString(value);
    }

    inline QString dateTimeDisplay(const std::string& value) {
        QString raw = QString::fromStdString(value);
        QDateTime dt = QDateTime::fromString(raw, "yyyy-MM-dd HH:mm:ss");
        return dt.isValid() ? dt.toString("dd/MM/yyyy HH:mm:ss") : raw;
    }
}
