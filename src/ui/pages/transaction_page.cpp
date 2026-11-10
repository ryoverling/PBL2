#include "transaction_page.h"
#include "ui_transaction_page.h"
#include "../gui_helpers.h"
#include "../main_window.h"
#include "../widgets/date_picker_popup.h"
#include "../../core/app/money_manager.h"
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QEvent>
#include <QButtonGroup>
#include <QSignalBlocker>

class TypeBadgeDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
        }

        bool income = index.data().toString().contains("Thu nhập");
        int pillH = 26;
        int pillW = qMin(option.rect.width() - 16, 100);
        int x = option.rect.x() + (option.rect.width() - pillW) / 2;
        int y = option.rect.y() + (option.rect.height() - pillH) / 2;
        QRect pill(x, y, pillW, pillH);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(income ? "#E7FAF3" : "#FFF0F3"));
        painter->drawRoundedRect(pill, 13, 13);
        painter->setPen(QColor(income ? "#059669" : "#E11D48"));
        QFont font = painter->font();
        font.setBold(true);
        font.setPointSize(9);
        painter->setFont(font);
        painter->drawText(pill, Qt::AlignCenter, income ? "↑ Thu nhập" : "↓ Chi tiêu");
        painter->restore();
    }
};

TransactionPage::TransactionPage(MoneyManager& mm, const std::string& dataDir, MainWindow* mainWin, QWidget *parent)
    : QWidget(parent), ui(new Ui::TransactionPage), moneyManager(mm), dataDirectory(dataDir), mainWindow(mainWin) {
    ui->setupUi(this);

    auto* header = ui->tblTransactions->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Fixed);
    header->setFixedHeight(40);
    ui->tblTransactions->setItemDelegateForColumn(0, new TypeBadgeDelegate(ui->tblTransactions));
    ui->tblTransactions->viewport()->installEventFilter(this);
    ui->tblTransactions->setTextElideMode(Qt::ElideRight);
    ui->tblTransactions->verticalHeader()->setVisible(false);
    ui->tblTransactions->verticalHeader()->setDefaultSectionSize(42);

    ui->dateFilterLayout->setSpacing(12);
    ui->dteFrom->setMinimumSize(125, 38);
    ui->dteTo->setMinimumSize(125, 38);

    // Gắn icon cho các ô lọc
    ui->cboType->setItemIcon(0, UiHelper::createFilterIcon());
    ui->cboType->setItemIcon(1, UiHelper::createArrowUpIcon());
    ui->cboType->setItemIcon(2, UiHelper::createArrowDownIcon());

    ui->btnAdd->setText(QString::fromUtf8("Thêm giao dịch"));
    ui->btnAdd->setIcon(UiHelper::createPlusIcon(Qt::white));
    ui->btnAdd->setIconSize(QSize(16, 16));
    ui->btnAdd->setProperty("role", "primary");
    ui->btnAdd->setCursor(Qt::PointingHandCursor);

    ui->btnEdit->setText(QString::fromUtf8("Sửa"));
    ui->btnEdit->setIcon(UiHelper::createEditIcon(QColor("#1769EE")));
    ui->btnEdit->setIconSize(QSize(16, 16));
    ui->btnEdit->setProperty("role", "secondary");
    ui->btnEdit->setToolTip("Chỉnh sửa giao dịch đang chọn");
    ui->btnEdit->setCursor(Qt::PointingHandCursor);

    ui->btnDelete->setText(QString::fromUtf8("Xóa"));
    ui->btnDelete->setIcon(UiHelper::createTrashIcon(QColor("#EF4765")));
    ui->btnDelete->setIconSize(QSize(16, 16));
    ui->btnDelete->setProperty("role", "danger");
    ui->btnDelete->setToolTip("Xóa giao dịch đang chọn");
    ui->btnDelete->setCursor(Qt::PointingHandCursor);

    resetDates();
    refreshData();
    updateColumnWidths();
}

void TransactionPage::updateColumnWidths() {
    int width = ui->tblTransactions->viewport()->width();
    if (width < 500) return;
    const int share[5] = {12, 17, 13, 17, 20};
    int used = 0;
    for (int i = 0; i < 5; ++i) {
        int column = width * share[i] / 100;
        ui->tblTransactions->setColumnWidth(i, column);
        used += column;
    }
    ui->tblTransactions->setColumnWidth(5, width - used);
}

void TransactionPage::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateColumnWidths();
}

bool TransactionPage::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->tblTransactions->viewport() && event->type() == QEvent::Resize) {
        updateColumnWidths();
    }
    return QWidget::eventFilter(watched, event);
}

TransactionPage::~TransactionPage() { delete ui; }

int TransactionPage::getSelectedTxId() {
    auto rows = ui->tblTransactions->selectionModel()->selectedRows();
    if (rows.size() != 1) return -1;
    auto* item = ui->tblTransactions->item(rows.first().row(), 0);
    return item ? item->data(Qt::UserRole).toInt() : -1;
}

void TransactionPage::on_tblTransactions_itemSelectionChanged() {
    bool selected = getSelectedTxId() > 0;
    ui->btnEdit->setEnabled(selected);
    ui->btnDelete->setEnabled(selected);
}

void TransactionPage::refreshData() {
    loadFilterCombos();
    populateTable();
    on_tblTransactions_itemSelectionChanged();
}

void TransactionPage::saveAndRefresh() {
    bool saved = moneyManager.saveAll(dataDirectory);
    if (!saved) UiHelper::showError(this, "Không lưu được dữ liệu.");
    if (mainWindow) mainWindow->refreshAllPages();
    else refreshData();
    if (saved) UiHelper::showInfo(this, "Đã lưu giao dịch.");
}

void TransactionPage::clearQuickFilterChip() {
    if (!ui->quickFilterGroup->checkedButton()) return;
    ui->quickFilterGroup->setExclusive(false);
    ui->quickFilterGroup->checkedButton()->setChecked(false);
    ui->quickFilterGroup->setExclusive(true);
}

void TransactionPage::applyQuickDateRange(const QDate& from, const QDate& to) {
    QSignalBlocker checkBlocker(ui->chkDateFilter);
    QSignalBlocker fromBlocker(ui->dteFrom);
    QSignalBlocker toBlocker(ui->dteTo);
    bool active = from.isValid() && to.isValid();
    ui->chkDateFilter->setChecked(active);
    if (active) { ui->dteFrom->setDate(from); ui->dteTo->setDate(to); }
    else resetDates();
    ui->dteFrom->setEnabled(active);
    ui->dteTo->setEnabled(active);
    refreshData();
}

void TransactionPage::filterByDate(const QDate& date) {
    clearQuickFilterChip();
    applyQuickDateRange(date, date);
}

void TransactionPage::on_btnQuickAll_clicked() { applyQuickDateRange(QDate(), QDate()); }

void TransactionPage::on_btnQuickThisMonth_clicked() {
    QDate today = QDate::currentDate();
    QDate first(today.year(), today.month(), 1);
    applyQuickDateRange(first, first.addMonths(1).addDays(-1));
}

void TransactionPage::on_btnQuickLastMonth_clicked() {
    QDate thisMonth = QDate::currentDate();
    QDate first = QDate(thisMonth.year(), thisMonth.month(), 1).addMonths(-1);
    applyQuickDateRange(first, first.addMonths(1).addDays(-1));
}

void TransactionPage::on_btnQuickSevenDays_clicked() {
    QDate today = QDate::currentDate();
    applyQuickDateRange(today.addDays(-6), today);
}

void TransactionPage::resetDates() {
    QSignalBlocker first(ui->dteFrom);
    QSignalBlocker last(ui->dteTo);
    QDate today = QDate::currentDate();
    ui->dteFrom->setDate(QDate(today.year(), today.month(), 1));
    ui->dteTo->setDate(today);
}

void TransactionPage::on_btnResetFilter_clicked() {
    QSignalBlocker keyword(ui->edtKeyword);
    QSignalBlocker type(ui->cboType);
    QSignalBlocker wallet(ui->cboWallet);
    QSignalBlocker category(ui->cboCategory);
    QSignalBlocker dateFilter(ui->chkDateFilter);
    ui->edtKeyword->clear();
    ui->cboType->setCurrentIndex(0);
    ui->cboWallet->setCurrentIndex(0);
    ui->cboCategory->setCurrentIndex(0);
    ui->chkDateFilter->setChecked(false);
    resetDates();
    on_chkDateFilter_toggled(false);
    ui->btnQuickAll->setChecked(true);
}

void TransactionPage::on_chkDateFilter_toggled(bool checked) {
    clearQuickFilterChip();
    if (checked && (!ui->dteFrom->date().isValid() || !ui->dteTo->date().isValid())) {
        resetDates();
    }
    ui->dteFrom->setEnabled(checked);
    ui->dteTo->setEnabled(checked);
    QString tip = checked ? "" : QString::fromUtf8("Bật 'Lọc theo ngày' để chọn");
    ui->dteFrom->setToolTip(tip);
    ui->dteTo->setToolTip(tip);
    refreshData();
}

void TransactionPage::on_dteFrom_dateChanged(const QDate& from) {
    if (from.isValid() && ui->dteTo->date().isValid() && from > ui->dteTo->date()) {
        ui->dteTo->setDate(from);
    }
    clearQuickFilterChip();
    refreshData();
}

void TransactionPage::on_dteTo_dateChanged(const QDate& to) {
    if (to.isValid() && ui->dteFrom->date().isValid() && to < ui->dteFrom->date()) {
        ui->dteFrom->setDate(to);
    }
    clearQuickFilterChip();
    refreshData();
}

void TransactionPage::on_cboType_currentIndexChanged(int) { refreshData(); }
void TransactionPage::on_cboWallet_currentIndexChanged(int) { refreshData(); }
void TransactionPage::on_cboCategory_currentIndexChanged(int) { refreshData(); }
void TransactionPage::on_edtKeyword_textChanged(const QString&) { refreshData(); }
