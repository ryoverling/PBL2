#include "statistics_page.h"
#include "ui_statistics_page.h"
#include "../../core/app/money_manager.h"
#include <cmath>

void StatisticsPage::updateComparison() {
    ExpenseComparison comparison = moneyManager.compareExpenseWithPreviousMonth(selectedMonth(), selectedYear());
    QString text = "Chưa có dữ liệu tháng trước";
    if (comparison.canCompare && comparison.diffAmount == 0) text = "Không đổi";
    else if (comparison.canCompare) {
        double pct = std::abs(comparison.diffPercent);
        QString pctStr = pct == std::floor(pct) ? QString::number(static_cast<long long>(pct))
                                                : QString::number(pct, 'f', 1);
        text = QString("%1 %2%").arg(QString::fromStdString(comparison.message), pctStr);
    }
    ui->lblComparison->setText("So với tháng trước: " + text);
}
