#pragma once

#include <QWidget>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QColor>

struct ChartSlice {
    QString label;
    double value;
    QColor color;
};

class DonutChartWidget : public QWidget {
private:
    QVector<ChartSlice> slices;

protected:
    void paintEvent(QPaintEvent* event) override;

public:
    explicit DonutChartWidget(QWidget* parent = nullptr);
    void setData(const QVector<ChartSlice>& data);
    QSize minimumSizeHint() const override;
};

class BarChartWidget : public QWidget {
private:
    QStringList labels;
    QVector<double> values;

protected:
    void paintEvent(QPaintEvent* event) override;

public:
    explicit BarChartWidget(QWidget* parent = nullptr);
    void setData(const QStringList& labels, const QVector<double>& values);
    QSize minimumSizeHint() const override;
};
