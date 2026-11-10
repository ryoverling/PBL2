#pragma once

#include <QWidget>
#include <string>
#include "../../core/structure/linked_list.h"

class MoneyManager;
class MainWindow;
class QDate;
class TransactionDialog;
class Transaction;
class QEvent;

namespace Ui {
class TransactionPage;
}

// Trang Quản Lý Giao Dịch
class TransactionPage : public QWidget {
    Q_OBJECT

public:
    explicit TransactionPage(MoneyManager& mm, const std::string& dataDir, MainWindow* mainWin = nullptr, QWidget *parent = nullptr);
    ~TransactionPage();

    void refreshData();
    void filterByDate(const QDate& date);

protected:
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void on_btnAdd_clicked();
    void on_btnEdit_clicked();
    void on_btnDelete_clicked();
    void on_btnResetFilter_clicked();
    void on_cboType_currentIndexChanged(int index);
    void on_cboWallet_currentIndexChanged(int index);
    void on_cboCategory_currentIndexChanged(int index);
    void on_edtKeyword_textChanged(const QString &text);
    void on_chkDateFilter_toggled(bool checked);
    void on_dteFrom_dateChanged(const QDate& date);
    void on_dteTo_dateChanged(const QDate& date);
    void on_tblTransactions_itemSelectionChanged();
    void on_btnQuickAll_clicked();
    void on_btnQuickThisMonth_clicked();
    void on_btnQuickLastMonth_clicked();
    void on_btnQuickSevenDays_clicked();

private:
    Ui::TransactionPage *ui;
    MoneyManager& moneyManager;
    std::string dataDirectory;
    MainWindow* mainWindow;

    void loadFilterCombos();
    void loadWalletFilter();
    void loadCategoryFilter();
    void populateTable();
    bool loadFilteredTransactions(LinkedList<Transaction*>& list);
    void updateSummary(const LinkedList<Transaction*>& list);
    void showEmptyTable();
    void addTxRow(const Transaction* tx);
    void updateColumnWidths();
    int getSelectedTxId();
    void saveAndRefresh();
    void resetDates();
    void clearQuickFilterChip();
    void applyQuickDateRange(const QDate& from, const QDate& to);
    bool updateTransaction(int id, const TransactionDialog& dialog);
};
