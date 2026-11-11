#include "plan_dialog.h"
#include "../gui_helpers.h"
#include "../../core/utils/utils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include <fstream>
#include <sstream>

bool PlanDialog::loadPlanFromFile(const std::string& userDir, BudgetPlan& plan) {
    plan = BudgetPlan();
    if (userDir.empty()) return false;
    std::ifstream file(userDir + "/plan.dat");
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string type;
        if (!std::getline(ss, type, '|')) continue;

        if (type == "budget") {
            std::string amountStr;
            if (std::getline(ss, amountStr)) {
                try { plan.monthlyBudget = std::stoll(amountStr); } catch (...) {}
            }
        } else if (type == "goal") {
            std::string curStr, tarStr;
            std::getline(ss, plan.goalName, '|');
            std::getline(ss, curStr, '|');
            std::getline(ss, tarStr, '|');
            std::getline(ss, plan.targetDate);
            try { plan.currentSaved = std::stoll(curStr); } catch (...) {}
            try { plan.targetAmount = std::stoll(tarStr); } catch (...) {}
        }
    }
    return true;
}

bool PlanDialog::savePlanToFile(const std::string& userDir, const BudgetPlan& plan) {
    if (userDir.empty()) return false;
    std::ofstream file(userDir + "/plan.dat", std::ios::trunc);
    if (!file.is_open()) return false;

    file << "budget|" << plan.monthlyBudget << "\n";
    file << "goal|" << plan.goalName << "|" << plan.currentSaved << "|" << plan.targetAmount << "|" << plan.targetDate << "\n";
    return true;
}

PlanDialog::PlanDialog(const BudgetPlan& initialPlan, QWidget* parent)
    : QDialog(parent), m_plan(initialPlan) {
    setWindowTitle("Kế hoạch & Mục tiêu tài chính");
    resize(460, 480);
    UiHelper::setupWindowTheme(this);
    setupUi();
}

void PlanDialog::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(16);

    // Tiêu đề
    auto* lblTitle = new QLabel("Thiết lập Kế hoạch tháng này", this);
    lblTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #0B2A6B;");
    mainLayout->addWidget(lblTitle);

    // Group 1: Ngân sách tháng
    auto* grpBudget = new QGroupBox("Hạn mức ngân sách chi tiêu tháng", this);
    grpBudget->setStyleSheet("QGroupBox { font-weight: 600; color: #0B2A6B; border: 1px solid #DCE8FA; border-radius: 12px; margin-top: 10px; padding-top: 14px; background: #FFFFFF; }");
    auto* budgetLayout = new QFormLayout(grpBudget);
    budgetLayout->setContentsMargins(14, 16, 14, 14);
    budgetLayout->setSpacing(10);

    edtBudget = new QLineEdit(this);
    edtBudget->setFixedHeight(38);
    edtBudget->setPlaceholderText("VD: 10000000");
    if (m_plan.monthlyBudget > 0) {
        edtBudget->setText(QString::number(m_plan.monthlyBudget));
    }
    budgetLayout->addRow("Ngân sách (đ):", edtBudget);
    mainLayout->addWidget(grpBudget);

    // Group 2: Mục tiêu tiết kiệm
    auto* grpGoal = new QGroupBox("Mục tiêu tiết kiệm", this);
    grpGoal->setStyleSheet("QGroupBox { font-weight: 600; color: #0B2A6B; border: 1px solid #DCE8FA; border-radius: 12px; margin-top: 10px; padding-top: 14px; background: #FFFFFF; }");
    auto* goalLayout = new QFormLayout(grpGoal);
    goalLayout->setContentsMargins(14, 16, 14, 14);
    goalLayout->setSpacing(10);

    edtGoalName = new QLineEdit(this);
    edtGoalName->setFixedHeight(38);
    edtGoalName->setPlaceholderText("VD: Quỹ khẩn cấp, Mua laptop...");
    edtGoalName->setText(QString::fromStdString(m_plan.goalName));

    edtCurrentSaved = new QLineEdit(this);
    edtCurrentSaved->setFixedHeight(38);
    edtCurrentSaved->setPlaceholderText("VD: 2000000");
    if (m_plan.currentSaved > 0) {
        edtCurrentSaved->setText(QString::number(m_plan.currentSaved));
    }

    edtTargetAmount = new QLineEdit(this);
    edtTargetAmount->setFixedHeight(38);
    edtTargetAmount->setPlaceholderText("VD: 15000000");
    if (m_plan.targetAmount > 0) {
        edtTargetAmount->setText(QString::number(m_plan.targetAmount));
    }

    edtTargetDate = new QLineEdit(this);
    edtTargetDate->setFixedHeight(38);
    edtTargetDate->setPlaceholderText("VD: 12/2026");
    edtTargetDate->setText(QString::fromStdString(m_plan.targetDate));

    goalLayout->addRow("Tên mục tiêu:", edtGoalName);
    goalLayout->addRow("Đã tiết kiệm (đ):", edtCurrentSaved);
    goalLayout->addRow("Số tiền mục tiêu (đ):", edtTargetAmount);
    goalLayout->addRow("Hạn dự kiến:", edtTargetDate);
    mainLayout->addWidget(grpGoal);

    // Footer buttons
    auto* footerLayout = new QHBoxLayout();
    footerLayout->setSpacing(10);

    btnDelete = new QPushButton("Xóa kế hoạch", this);
    btnDelete->setFixedHeight(40);
    btnDelete->setCursor(Qt::PointingHandCursor);
    btnDelete->setStyleSheet(
        "QPushButton { color: #E0173D; background-color: #FFFFFF; border: 1.5px solid #FFCCD5; "
        "border-radius: 10px; padding: 0 14px; font-weight: 600; font-size: 13px; } "
        "QPushButton:hover { background-color: #FFF0F3; }"
    );
    btnDelete->setVisible(m_plan.hasPlan());

    btnCancel = new QPushButton("Hủy", this);
    btnCancel->setFixedHeight(40);
    btnCancel->setMinimumWidth(80);
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnCancel->setStyleSheet(
        "QPushButton { color: #5B6B8C; background-color: #FFFFFF; border: 1.5px solid #D5E1F5; "
        "border-radius: 10px; padding: 0 16px; font-weight: 600; font-size: 13px; } "
        "QPushButton:hover { background-color: #F1F6FF; }"
    );

    btnSave = new QPushButton("Lưu kế hoạch", this);
    btnSave->setFixedHeight(40);
    btnSave->setMinimumWidth(110);
    btnSave->setCursor(Qt::PointingHandCursor);
    btnSave->setStyleSheet(
        "QPushButton { color: #FFFFFF; background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1E6FFF, stop:1 #155EEF); "
        "border: none; border-radius: 10px; padding: 0 18px; font-weight: 700; font-size: 13px; } "
        "QPushButton:hover { background: #155EEF; }"
    );

    footerLayout->addWidget(btnDelete);
    footerLayout->addStretch(1);
    footerLayout->addWidget(btnCancel);
    footerLayout->addWidget(btnSave);
    mainLayout->addLayout(footerLayout);

    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnSave, &QPushButton::clicked, this, &PlanDialog::onSaveClicked);
    connect(btnDelete, &QPushButton::clicked, this, &PlanDialog::onDeleteClicked);
}

void PlanDialog::onSaveClicked() {
    bool ok = false;
    long long b = edtBudget->text().trimmed().toLongLong(&ok);
    m_plan.monthlyBudget = (ok && b >= 0) ? b : 0;

    m_plan.goalName = edtGoalName->text().trimmed().toStdString();
    long long cs = edtCurrentSaved->text().trimmed().toLongLong(&ok);
    m_plan.currentSaved = (ok && cs >= 0) ? cs : 0;

    long long ta = edtTargetAmount->text().trimmed().toLongLong(&ok);
    m_plan.targetAmount = (ok && ta >= 0) ? ta : 0;

    m_plan.targetDate = edtTargetDate->text().trimmed().toStdString();

    accept();
}

void PlanDialog::onDeleteClicked() {
    if (UiHelper::askConfirmation(this, "Xác nhận xóa", "Bạn có chắc chắn muốn xóa kế hoạch và mục tiêu này?")) {
        m_plan = BudgetPlan();
        accept();
    }
}

