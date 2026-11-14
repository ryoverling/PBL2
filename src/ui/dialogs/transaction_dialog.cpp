#include "transaction_dialog.h"
#include "ui_transaction_dialog.h"
#include "../gui_helpers.h"
#include "../gui_icons_kpi.h"
#include "../gui_icons_nav.h"
#include "../widgets/date_picker_popup.h"
#include "../../core/app/money_manager.h"
#include "../../core/model/transaction.h"
#include <QCompleter>
#include <QButtonGroup>
#include <QMessageBox>
#include <QStyle>

TransactionDialog::TransactionDialog(MoneyManager& mm, int txId, QWidget *parent)
    : QDialog(parent), ui(new Ui::TransactionDialog), moneyManager(mm), editingTxId(txId) {
    ui->setupUi(this);
    UiHelper::setupWindowTheme(this);
    setMinimumWidth(540);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    ui->lblAmountWarning->hide();
    ui->lblCategoryHint->hide();

    typeGroup = new QButtonGroup(this);
    typeGroup->addButton(ui->btnTypeExpense, 0);
    typeGroup->addButton(ui->btnTypeIncome, 1);
    ui->btnTypeExpense->setText("Khoản chi");
    ui->btnTypeExpense->setIcon(GuiIcons::createArrowDownIcon(QColor("#E0173D")));
    ui->btnTypeIncome->setText("Khoản thu");
    ui->btnTypeIncome->setIcon(GuiIcons::createArrowUpIcon(QColor("#0E8F4F")));
    updateSegmentedStyle();

    ui->edtAmount->addAction(QIcon(GuiIcons::createCoinHeaderIcon()), QLineEdit::TrailingPosition);
    loadWallets();
    loadCategories();
    ui->lblDialogTitle->setText(txId > 0 ? "Sửa giao dịch" : "Thêm giao dịch mới");
    ui->btnCancel->setIcon(GuiIcons::createTrashIcon(QColor("#5B6B8C")));
    ui->btnSave->setIcon(GuiIcons::createPlusIcon(Qt::white));

    if (txId > 0) {
        setupEditData(moneyManager.getTransactionManager().findById(txId));
    } else {
        ui->dteDate->setDate(QDate::currentDate());
    }

    connect(ui->edtAmount, &QLineEdit::textChanged, this, &TransactionDialog::validateLiveInput);
    connect(ui->cboWallet, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TransactionDialog::validateLiveInput);
    connect(ui->dteDate, &DatePickerField::dateChanged, this, &TransactionDialog::validateLiveInput);
    validateLiveInput();
}

TransactionDialog::~TransactionDialog() {
    delete ui;
}

void TransactionDialog::updateSegmentedStyle() {
    bool isExpense = ui->btnTypeExpense->isChecked();
    ui->btnTypeExpense->setProperty("active", isExpense);
    ui->btnTypeIncome->setProperty("active", !isExpense);
    ui->btnTypeExpense->style()->unpolish(ui->btnTypeExpense);
    ui->btnTypeExpense->style()->polish(ui->btnTypeExpense);
    ui->btnTypeIncome->style()->unpolish(ui->btnTypeIncome);
    ui->btnTypeIncome->style()->polish(ui->btnTypeIncome);
}

void TransactionDialog::setInitialType(TransactionType type) {
    if (type == TransactionType::INCOME) {
        ui->btnTypeIncome->setChecked(true);
    } else {
        ui->btnTypeExpense->setChecked(true);
    }
    updateSegmentedStyle();
    loadCategories();
    validateLiveInput();
}

void TransactionDialog::on_btnTypeExpense_clicked() {
    updateSegmentedStyle();
    loadCategories();
    validateLiveInput();
}

void TransactionDialog::on_btnTypeIncome_clicked() {
    updateSegmentedStyle();
    loadCategories();
    validateLiveInput();
}

void TransactionDialog::setupEditData(const Transaction* tx) {
    if (!tx) return;
    setInitialType(tx->getType());
    loadCategories();
    ui->edtAmount->setText(QString::number(tx->getAmount()));
    ui->dteDate->setDate(QDate(tx->getDate().year, tx->getDate().month, tx->getDate().day));
    ui->edtNote->setText(QString::fromStdString(tx->getNote()));
    ui->cboWallet->setCurrentIndex(ui->cboWallet->findData(tx->getWalletId()));
    const Category* category = moneyManager.getCategoryManager().findById(tx->getCategoryId());
    if (category) {
        ui->cboCategory->setEditText(QString::fromStdString(category->getName()));
    }

    // Ở chế độ Sửa, khóa lựa chọn Khoản chi/Khoản thu để không đổi loại giao dịch:
    // 1. Về OOP: Transaction là lớp trừu tượng có 2 lớp con là Income và Expense.
    //    Đổi loại giao dịch đòi hỏi phải hủy đối tượng cũ và cấp phát đối tượng mới, phá vỡ tính nguyên vẹn.
    // 2. Về nghiệp vụ: Đổi chiều thu/chi làm biến động kép dòng tiền lịch sử, dễ vi phạm bất biến số dư ví >= 0.
    // 3. Quy trình an toàn và rõ ràng nhất cho người dùng: Muốn đổi loại, hãy xóa giao dịch cũ và tạo mới.
    ui->btnTypeExpense->setEnabled(false);
    ui->btnTypeIncome->setEnabled(false);
    const QString lockTooltip = "Muốn đổi loại, hãy xóa và thêm giao dịch mới";
    ui->btnTypeExpense->setToolTip(lockTooltip);
    ui->btnTypeIncome->setToolTip(lockTooltip);
}

void TransactionDialog::loadWallets() {
    ui->cboWallet->clear();
    const auto& wallets = moneyManager.getWalletManager().getWallets();
    for (const Node<Wallet>* cur = wallets.getHead(); cur; cur = cur->next) {
        ui->cboWallet->addItem(QString::fromStdString(cur->data.getName()), cur->data.getId());
    }
}

void TransactionDialog::loadCategories() {
    ui->cboCategory->clear();
    ui->cboCategory->setInsertPolicy(QComboBox::NoInsert);
    CategoryType type = getType() == TransactionType::INCOME ? CategoryType::INCOME : CategoryType::EXPENSE;
    const auto& categories = moneyManager.getCategoryManager().getCategories();
    QStringList names;
    for (const Node<Category>* cur = categories.getHead(); cur; cur = cur->next) {
        if (cur->data.getType() == type) {
            QString nameStr = QString::fromStdString(cur->data.getName());
            ui->cboCategory->addItem(nameStr, cur->data.getId());
            names << nameStr;
        }
    }
    ui->cboCategory->setEditText("");
    ui->lblCategoryHint->clear();
    ui->lblCategoryHint->hide();

    // Cấu hình Completer Popup chuẩn để TUYỆT ĐỐI KHÔNG làm lỗi bộ gõ tiếng Việt (Telex/VNI)
    QCompleter* comp = new QCompleter(names, ui->cboCategory);
    comp->setCaseSensitivity(Qt::CaseInsensitive);
    comp->setFilterMode(Qt::MatchContains);
    comp->setCompletionMode(QCompleter::PopupCompletion);
    comp->setWrapAround(false);
    ui->cboCategory->setCompleter(comp);
}

const Category* TransactionDialog::findCategory(const QString& name) const {
    CategoryType type = getType() == TransactionType::INCOME ? CategoryType::INCOME : CategoryType::EXPENSE;
    const auto& categories = moneyManager.getCategoryManager().getCategories();
    for (const Node<Category>* cur = categories.getHead(); cur; cur = cur->next) {
        QString candidate = UiHelper::cleanName(QString::fromStdString(cur->data.getName()));
        if (cur->data.getType() == type && candidate.compare(name, Qt::CaseInsensitive) == 0) {
            return &cur->data;
        }
    }
    return nullptr;
}

void TransactionDialog::on_cboCategory_editTextChanged(const QString& text) {
    QString name = UiHelper::cleanName(text);
    bool existing = name.isEmpty() || findCategory(name);
    ui->lblCategoryHint->setText(existing ? "" : "Danh mục mới sẽ được tạo khi lưu giao dịch");
    ui->lblCategoryHint->setVisible(!existing);
}

void TransactionDialog::validateLiveNegativeWallet(long long curBal, const QString& walletName, bool isExpense, long long amt) {
    ui->lblWalletBalance->setText(QString("⚠ Số dư khả dụng: %1 (Ví đang âm)").arg(UiHelper::formatMoneyVND(curBal)));
    ui->lblWalletBalance->setStyleSheet("color: #EF4765; font-size: 12px; font-weight: 600; padding: 2px 0;");

    if (isExpense) {
        ui->lblAmountWarning->setText(QString("⚠ Ví %1 đang âm %2, cần đối soát dữ liệu trước khi chi thêm").arg(walletName, UiHelper::formatMoneyVND(std::abs(curBal))));
        ui->lblAmountWarning->setStyleSheet("color: #EF4765; font-size: 12px; font-weight: 600; line-height: 130%;");
        ui->lblAmountWarning->show();
        ui->btnSave->setEnabled(false);
    } else {
        ui->lblAmountWarning->setText("ℹ Khoản thu sẽ giúp bù đắp số dư âm của ví này.");
        ui->lblAmountWarning->setStyleSheet("color: #059669; font-size: 12px; font-weight: 500;");
        ui->lblAmountWarning->show();
        ui->btnSave->setEnabled(amt > 0);
    }
}

void TransactionDialog::validateLiveNormalWallet(long long curBal, bool isExpense, long long amt) {
    if (isExpense) {
        ui->lblWalletBalance->setText(QString("Số dư khả dụng: %1").arg(UiHelper::formatMoneyVND(curBal)));
        ui->lblWalletBalance->setStyleSheet("color: #536986; font-size: 12px; padding: 2px 0;");

        if (amt > curBal) {
            long long lack = amt - curBal;
            ui->lblAmountWarning->setText(QString("⚠ Vượt quá số dư khả dụng (còn thiếu %1)").arg(UiHelper::formatMoneyVND(lack)));
            ui->lblAmountWarning->setStyleSheet("color: #EF4765; font-size: 12px; font-weight: 600; line-height: 130%;");
            ui->lblAmountWarning->show();
            ui->btnSave->setEnabled(false);
        } else {
            ui->lblAmountWarning->clear();
            ui->lblAmountWarning->hide();
            ui->btnSave->setEnabled(amt > 0);
        }
    } else {
        ui->lblWalletBalance->setText(QString("Số dư hiện tại: %1").arg(UiHelper::formatMoneyVND(curBal)));
        ui->lblWalletBalance->setStyleSheet("color: #536986; font-size: 12px; padding: 2px 0;");
        ui->lblAmountWarning->clear();
        ui->lblAmountWarning->hide();
        ui->btnSave->setEnabled(amt > 0);
    }
}

void TransactionDialog::validateLiveInput() {
    int walletId = getWalletId();
    const Wallet* w = moneyManager.getWalletManager().findById(walletId);
    long long curBal = w ? w->getBalance() : 0;
    QString walletName = w ? QString::fromStdString(w->getName()) : "Ví";
    bool isExpense = (getType() == TransactionType::EXPENSE);
    long long amt = getAmount();

    if (curBal < 0) {
        validateLiveNegativeWallet(curBal, walletName, isExpense, amt);
    } else {
        validateLiveNormalWallet(curBal, isExpense, amt);
    }
}

bool TransactionDialog::validateInputFields() {
    QString name = UiHelper::cleanName(ui->cboCategory->currentText());
    if (getAmount() <= 0 || ui->cboWallet->currentIndex() < 0) {
        UiHelper::showError(this, "Vui lòng nhập số tiền hợp lệ và chọn ví thanh toán.");
        return false;
    }
    if (!ui->dteDate->isValidDate() || !getDate().isValid()) {
        UiHelper::showError(this, "Ngày giao dịch không hợp lệ. Vui lòng kiểm tra lại.");
        return false;
    }
    if (!UiHelper::isValidName(name) || getNote().find('|') != std::string::npos) {
        UiHelper::showError(this, "Danh mục hoặc ghi chú chứa ký tự không hợp lệ.");
        return false;
    }
    return true;
}

void TransactionDialog::on_btnSave_clicked() {
    if (!validateInputFields()) return;

    QString name = UiHelper::cleanName(ui->cboCategory->currentText());
    const Category* category = findCategory(name);
    resolvedCategoryName = category ? category->getName() : name.toStdString();

    CategoryType catType = (getType() == TransactionType::INCOME) ? CategoryType::INCOME : CategoryType::EXPENSE;
    BalanceValidationResult valRes;

    if (editingTxId > 0) {
        valRes = moneyManager.validateUpdateTransaction(editingTxId, getAmount(), getDate(), getWalletId(),
                                                        resolvedCategoryName, catType);
    } else {
        Transaction* testTx = createTransaction(1, 1);
        valRes = moneyManager.validateAddTransaction(testTx);
        delete testTx;
    }

    if (!valRes.isValid) {
        UiHelper::showBalanceError(this, valRes);
        return;
    }

    accept();
}

long long TransactionDialog::getAmount() const {
    QString text = ui->edtAmount->text().trimmed();
    text.remove('.');
    text.remove(',');
    text.remove(' ');
    bool valid = false;
    long long amount = text.toLongLong(&valid);
    return valid ? amount : 0;
}

Date TransactionDialog::getDate() const {
    QDate date = ui->dteDate->date();
    return Date(date.day(), date.month(), date.year());
}

TransactionType TransactionDialog::getType() const {
    return (ui->btnTypeIncome && ui->btnTypeIncome->isChecked()) ? TransactionType::INCOME : TransactionType::EXPENSE;
}

int TransactionDialog::getWalletId() const {
    return ui->cboWallet->currentData().toInt();
}

std::string TransactionDialog::getNote() const {
    return ui->edtNote->text().trimmed().toStdString();
}

void TransactionDialog::on_btnCancel_clicked() {
    reject();
}

Transaction* TransactionDialog::createTransaction(int id, int categoryId) const {
    if (getType() == TransactionType::INCOME) {
        return new Income(id, getAmount(), getDate(), getNote(), getWalletId(), categoryId);
    }
    return new Expense(id, getAmount(), getDate(), getNote(), getWalletId(), categoryId);
}
