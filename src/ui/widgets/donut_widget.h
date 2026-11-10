#pragma once

#include <QWidget>
#include <QColor>
#include <QString>
#include "../../core/utils/stat_utils.h"

class QPainter;

class DonutWidget : public QWidget {
    Q_OBJECT
public:
    explicit DonutWidget(QWidget* parent = nullptr);
    void setData(const LinkedList<CategoryExpense>& categories, long long totalExpense);
    int sliceCount() const { return count; }
    QString sliceName(int index) const { return slices[index].name; }
    long long sliceAmount(int index) const { return slices[index].amount; }
    QColor sliceColor(int index) const { return slices[index].color; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    struct Slice {
        QString name;
        long long amount = 0;
        int percentage = 0;
        QColor color;
    };
    Slice slices[6];
    int count = 0;
    long long total = 0;
    void appendCategory(const CategoryExpense& item);
    void drawDonut(QPainter& painter);
    void drawPercentageLabels(QPainter& painter, const QRectF& circle, int diameter,
                              const int starts[], const int sweeps[]);
};
