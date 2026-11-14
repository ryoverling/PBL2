#include "transaction_page.h"
#include "ui_transaction_page.h"
#include "../gui_helpers.h"
#include "../gui_icons_nav.h"
#include "../gui_icons_kpi.h"
#include "../main_window.h"
#include "../dialogs/transaction_dialog.h"
#include "../widgets/date_picker_popup.h"
#include "../../core/app/money_manager.h"
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QEvent>
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
        painter->setBrush(QColor(income ? "#E6FAF3" : "#FEECEB"));
        painter->drawRoundedRect(pill, 13, 13);
        painter->setPen(QColor(income ? "#0E8F4F" : "#E0173D"));
        QFont font = painter->font();
        font.setBold(true);
        font.setPointSize(9);
        painter->setFont(font);
        painter->drawText(pill, Qt::AlignCenter, income ? "↑ Thu nhập" : "↓ Chi tiêu");
        painter->restore();
    }
};

void TransactionPage::initHeaderAndActions() {
    ui->lblHeaderIcon->setPixmap(GuiIcons::createExchangeBadge(44));
    ui->btnAdd->setText(QString::fromUtf8("Thêm giao dịch"));
    ui->btnAdd->setIcon(UiHelper::createPlusIcon(Qt::white));
    ui->btnAdd->setIconSize(QSize(14, 14));
    ui->btnAdd->setCursor(Qt::PointingHandCursor);

    ui->btnEdit->setText(QString::fromUtf8("Sửa"));
    ui->btnEdit->setIcon(UiHelper::createEditIcon(QColor("#0B2A6B")));
    ui->btnEdit->setIconSize(QSize(16, 16));
    ui->btnEdit->setToolTip("Chỉnh sửa giao dịch đang chọn");
    ui->btnEdit->setCursor(Qt::PointingHandCursor);

    ui->btnDelete->setText(QString::fromUtf8("Xóa"));
    ui->btnDelete->setIcon(UiHelper::createTrashIcon(QColor("#E0173D")));
    ui->btnDelete->setIconSize(QSize(16, 16));
    ui->btnDelete->setToolTip("Xóa giao dịch đang chọn");
    ui->btnDelete->setCursor(Qt::PointingHandCursor);
}

void TransactionPage::initSummaryCards() {
    ui->iconIncomeBadge->setPixmap(GuiIcons::createKpiBadge("income", 44));
    ui->iconExpenseBadge->setPixmap(GuiIcons::createKpiBadge("expense", 44));
    ui->iconCountBadge->setPixmap(GuiIcons::createKpiBadge("count", 44));

    ui->lblIncomeSparkline->setPixmap(GuiIcons::createWaveSparkline(true, 80, 36));
    ui->lblExpenseSparkline->setPixmap(GuiIcons::createWaveSparkline(false, 80, 36));
    ui->lblCountMiniBars->setPixmap(GuiIcons::createMiniBars(80, 36, QColor("#1E6FFF")));
}

void TransactionPage::initTableAndFilters() {
    ui->tblTransactions->setShowGrid(false);
    ui->tblTransactions->setIconSize(QSize(24, 24));
    auto* header = ui->tblTransactions->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Fixed);
    header->setFixedHeight(44);
    ui->tblTransactions->setItemDelegateForColumn(0, new TypeBadgeDelegate(ui->tblTransactions));
    ui->tblTransactions->viewport()->installEventFilter(this);
    ui->tblTransactions->setTextElideMode(Qt::ElideRight);
    ui->tblTransactions->verticalHeader()->setVisible(false);
    ui->tblTransactions->verticalHeader()->setDefaultSectionSize(48);

    ui->tblTransactions->horizontalHeaderItem(0)->setIcon(GuiIcons::createSortHeaderIcon());
    ui->tblTransactions->horizontalHeaderItem(1)->setIcon(GuiIcons::createCoinHeaderIcon());
    ui->tblTransactions->horizontalHeaderItem(2)->setIcon(GuiIcons::createCalendarHeaderIcon());
    ui->tblTransactions->horizontalHeaderItem(3)->setIcon(GuiIcons::createWalletHeaderIcon());
    ui->tblTransactions->horizontalHeaderItem(4)->setIcon(GuiIcons::createTagHeaderIcon());
    ui->tblTransactions->horizontalHeaderItem(5)->setIcon(GuiIcons::createNoteHeaderIcon());

    ui->cboType->setItemIcon(0, UiHelper::createFilterIcon());
    ui->cboType->setItemIcon(1, UiHelper::createArrowUpIcon());
    ui->cboType->setItemIcon(2, UiHelper::createArrowDownIcon());

    ui->btnResetFilter->setIcon(GuiIcons::createResetIcon(14, QColor("#1E6FFF")));
    ui->btnResetFilter->setIconSize(QSize(14, 14));
    ui->btnResetFilter->setCursor(Qt::PointingHandCursor);
}

TransactionPage::TransactionPage(MoneyManager& mm, const std::string& dataDir, MainWindow* mainWin, QWidget *parent)
    : QWidget(parent), ui(new Ui::TransactionPage), moneyManager(mm), dataDirectory(dataDir), mainWindow(mainWin) {
    ui->setupUi(this);

    UiHelper::applyCardShadow(ui->frmFilter);
    UiHelper::applyCardShadow(ui->cardFilteredIncome);
    UiHelper::applyCardShadow(ui->cardFilteredExpense);
    UiHelper::applyCardShadow(ui->cardFilteredCount);
    UiHelper::applyCardShadow(ui->frmTableCard);

    initHeaderAndActions();
    initSummaryCards();
    initTableAndFilters();

    resetDates();
    refreshData();
    updateColumnWidths();
}

void TransactionPage::updateColumnWidths() {
    int width = ui->tblTransactions->viewport()->width();
    if (width < 500) return;
    const int share[5] = {14, 20, 14, 18, 18};
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
    QModelIndexList rows = ui->tblTransactions->selectionModel()->selectedRows();
    if (rows.size() != 1) return -1;
    QTableWidgetItem* item = ui->tblTransactions->item(rows.first().row(), 0);
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
    QSignalBlocker fromBlocker(ui->dteFrom);
    QSignalBlocker toBlocker(ui->dteTo);
    if (from.isValid() && to.isValid()) {
        ui->dteFrom->setDate(from);
        ui->dteTo->setDate(to);
    } else {
        ui->dteFrom->clearDate();
        ui->dteTo->clearDate();
    }
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
    ui->dteFrom->clearDate();
    ui->dteTo->clearDate();
}

void TransactionPage::on_btnResetFilter_clicked() {
    QSignalBlocker keyword(ui->edtKeyword);
    QSignalBlocker type(ui->cboType);
    QSignalBlocker wallet(ui->cboWallet);
    QSignalBlocker category(ui->cboCategory);
    ui->edtKeyword->clear();
    ui->cboType->setCurrentIndex(0);
    ui->cboWallet->setCurrentIndex(0);
    ui->cboCategory->setCurrentIndex(0);
    resetDates();
    ui->btnQuickAll->setChecked(true);
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

void TransactionPage::on_btnAdd_clicked() {
    TransactionDialog dialog(moneyManager, -1, this);
    if (dialog.exec() != QDialog::Accepted) return;
    int id = moneyManager.getTransactionManager().getNextId();
    Transaction* transaction = dialog.createTransaction(id, 0);
    CategoryType type = dialog.getType() == TransactionType::INCOME ? CategoryType::INCOME : CategoryType::EXPENSE;
    if (moneyManager.addTransaction(transaction, dialog.getCategoryName(), type)) {
        saveAndRefresh();
    } else {
        delete transaction;
        UiHelper::showError(this, "Không thể thêm giao dịch (số dư ví không đủ hoặc dữ liệu không hợp lệ).");
    }
}

bool TransactionPage::updateTransaction(int id, const TransactionDialog& dialog) {
    CategoryType type = dialog.getType() == TransactionType::INCOME ? CategoryType::INCOME : CategoryType::EXPENSE;
    return moneyManager.updateTransaction(id, dialog.getAmount(), dialog.getDate(), dialog.getNote(),
                                           dialog.getWalletId(), dialog.getCategoryName(), type);
}

void TransactionPage::on_btnEdit_clicked() {
    int id = getSelectedTxId();
    if (id < 0) return;
    TransactionDialog dialog(moneyManager, id, this);
    if (dialog.exec() != QDialog::Accepted) return;
    if (updateTransaction(id, dialog)) {
        saveAndRefresh();
    } else {
        UiHelper::showError(this, "Không thể cập nhật giao dịch (vi phạm bất biến số dư hoặc lỗi dữ liệu).");
    }
}

void TransactionPage::on_btnDelete_clicked() {
    int id = getSelectedTxId();
    if (id < 0) return;
    const Transaction* tx = moneyManager.getTransactionManager().findById(id);
    if (!tx) return;

    BalanceValidationResult valRes = moneyManager.validateDeleteTransaction(id);
    if (!valRes.isValid) {
        UiHelper::showBalanceError(this, valRes);
        return;
    }

    QString type = tx->getType() == TransactionType::INCOME ? "Thu" : "Chi";
    QString note = tx->getNote().empty() ? "Không có" : QString::fromStdString(tx->getNote());
    QString message = QString("%1 | %2 | %3\nGhi chú: %4\n\nXóa giao dịch này?")
        .arg(type).arg(UiHelper::formatDate(tx->getDate())).arg(UiHelper::formatMoneyVND(tx->getAmount())).arg(note);
    if (!UiHelper::askConfirmation(this, "Xác nhận xóa", message)) return;
    if (moneyManager.deleteTransaction(id)) {
        saveAndRefresh();
    } else {
        UiHelper::showError(this, "Không thể xóa giao dịch.");
    }
}
