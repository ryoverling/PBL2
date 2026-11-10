#pragma once

#include <QWidget>
#include <QList>
#include <string>

class MoneyManager;
class MainWindow;
class WalletCard;
class AddWalletCard;
class Wallet;

namespace Ui {
class WalletPage;
}

// Trang Quản Lý Ví Tiền
class WalletPage : public QWidget {
    Q_OBJECT

public:
    explicit WalletPage(MoneyManager& mm, const std::string& dataDir, MainWindow* mainWin = nullptr, QWidget *parent = nullptr);
    ~WalletPage();

    void refreshData();

private slots:
    void on_btnAdd_clicked();
    void on_btnEdit_clicked();
    void on_btnDelete_clicked();
    void on_btnEye_clicked();
    void on_btnEmptyAdd_clicked();
    void onWalletCardClicked(int walletId);

private:
    Ui::WalletPage *ui;
    MoneyManager& moneyManager;
    std::string dataDirectory;
    MainWindow* mainWindow;
    QList<WalletCard*> cards;
    AddWalletCard* addCard = nullptr;
    int selectedWalletId = -1;
    bool reflowing = false;

    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void populateCards();
    void addWalletCard(const Wallet& wallet);
    void reflowCards();
    int getSelectedWalletId() const;
    void updateSelection();
    void saveAndRefresh();
};
