#pragma once

#include <QWidget>
#include <QColor>
#include <QString>
#include "../../core/utils/utils.h"

class QPainter;

class DonutWidget : public QWidget {
    Q_OBJECT
public:
    explicit DonutWidget(QWidget* parent = nullptr);
    void setData(const LinkedList<CategoryExpense>& categories, long long totalExpense);
    int sliceCount() const { return count; }
    QString sliceName(int index) const { return slices[index].name; }
    long long sliceAmount(int index) const { return slices[index].amount; }
    int slicePercentage(int index) const { return slices[index].percentage; }
    QColor sliceColor(int index) const { return slices[index].color; }
    QSize sizeHint() const override { return QSize(260, 260); }
    QSize minimumSizeHint() const override { return QSize(220, 220); }

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
    void drawCenterHole(QPainter& painter, const QRectF& circle, int diameter, int inset);
    void drawPercentageLabels(QPainter& painter, const QRectF& circle, int diameter,
                              const int starts[], const int sweeps[]);
};
