#pragma once

#include <QDialog>
#include <string>

struct BudgetPlan {
    long long monthlyBudget = 0; // Ngân sách chi tiêu tháng (VND)
    std::string goalName;        // Tên mục tiêu tiết kiệm
    long long currentSaved = 0;  // Số tiền đã tiết kiệm được
    long long targetAmount = 0;  // Số tiền mục tiêu
    std::string targetDate;      // Hạn dự kiến (ví dụ: "12/2026")

    bool hasPlan() const {
        return monthlyBudget > 0 || (!goalName.empty() && targetAmount > 0);
    }
};

class QLineEdit;
class QPushButton;

class PlanDialog : public QDialog {
    Q_OBJECT

public:
    explicit PlanDialog(const BudgetPlan& initialPlan, QWidget* parent = nullptr);
    ~PlanDialog() override = default;

    BudgetPlan getPlan() const { return m_plan; }
    static bool loadPlanFromFile(const std::string& userDir, BudgetPlan& plan);
    static bool savePlanToFile(const std::string& userDir, const BudgetPlan& plan);

private slots:
    void onSaveClicked();
    void onDeleteClicked();

private:
    void setupUi();

    BudgetPlan m_plan;
    QLineEdit* edtBudget = nullptr;
    QLineEdit* edtGoalName = nullptr;
    QLineEdit* edtCurrentSaved = nullptr;
    QLineEdit* edtTargetAmount = nullptr;
    QLineEdit* edtTargetDate = nullptr;

    QPushButton* btnDelete = nullptr;
    QPushButton* btnCancel = nullptr;
    QPushButton* btnSave = nullptr;
};
