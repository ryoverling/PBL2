#pragma once

#include <QString>
#include <QLocale>
#include <QDate>
#include <QDateTime>
#include <QEvent>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QAbstractItemModel>
#include <QDialogButtonBox>

#include "../model/wallet.h"
#include "../model/category.h"

namespace UiHelpers {
    inline void prepareDialogButtons(QDialogButtonBox* buttons) {
        buttons->button(QDialogButtonBox::Save)->setText("Lưu");
        buttons->button(QDialogButtonBox::Save)->setDefault(true);
        buttons->button(QDialogButtonBox::Cancel)->setText("Hủy");
    }
    // Width proportions and minimums belong to the Designer form. This adapter
    // only responds to window resizing; it does not create or style widgets.
    class TableSizing final : public QObject {
    public:
        explicit TableSizing(QTableWidget* table) : QObject(table), table(table) {
            table->viewport()->installEventFilter(this);
        }
        bool eventFilter(QObject*, QEvent* event) override {
            if (event->type() == QEvent::Resize || event->type() == QEvent::Show) resizeColumns();
            return false;
        }
        void resizeColumns() {
            if (resizing) return;
            const auto weights = table->property("columnWeights").toString().split(',');
            const auto minimums = table->property("columnMinimumWidths").toString().split(',');
            if (weights.size() != table->columnCount() || minimums.size() != weights.size()) return;
            resizing = true;
            int minimumTotal = 0, weightTotal = 0;
            for (int i = 0; i < weights.size(); ++i) {
                minimumTotal += minimums[i].toInt(); weightTotal += weights[i].toInt();
            }
            const int extra = qMax(0, table->viewport()->width() - minimumTotal);
            for (int i = 0; i < weights.size(); ++i)
                table->setColumnWidth(i, minimums[i].toInt() + extra * weights[i].toInt() / qMax(1, weightTotal));
            resizing = false;
        }
    private:
        QTableWidget* table;
        bool resizing = false;
    };

    inline void prepareTable(QTableWidget* table) {
        auto sizing = new TableSizing(table);
        sizing->resizeColumns();
    }

    inline void bindTableActions(QTableWidget* table, QPushButton* edit, QPushButton* remove,
                                 QLabel* status, QLabel* empty) {
        prepareTable(table);
        const auto update = [=]() {
            const bool selected = !table->selectedItems().isEmpty();
            edit->setEnabled(selected);
            remove->setEnabled(selected);
            empty->setVisible(table->rowCount() == 0);
            status->setText(QString("%1 mục • %2").arg(table->rowCount()).arg(
                selected ? "Đã chọn 1 dòng" : "Chọn một dòng để sửa hoặc xóa"));
        };
        QObject::connect(table, &QTableWidget::itemSelectionChanged, table, update);
        QObject::connect(table->model(), &QAbstractItemModel::rowsInserted, table, update);
        QObject::connect(table->model(), &QAbstractItemModel::rowsRemoved, table, update);
        QObject::connect(table->model(), &QAbstractItemModel::modelReset, table, update);
        update();
    }

    inline void completeTable(QTableWidget* table, const QList<int>& moneyColumns = {}) {
        for (int row = 0; row < table->rowCount(); ++row) {
            for (int col = 0; col < table->columnCount(); ++col) {
                auto item = table->item(row, col);
                if (!item) continue;
                item->setToolTip(item->text());
                if (moneyColumns.contains(col)) item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            }
        }
    }

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
