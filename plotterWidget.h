#ifndef PLOTTERWIDGET_H
#define PLOTTERWIDGET_H

#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QScatterSeries>
#include <QCheckBox>

#include "uavcandecoder.h"

class Plotter : public QWidget
{
    Q_OBJECT
public:
    explicit Plotter(QWidget *parent = nullptr);

signals:


public slots:
    void payloadReceived(QString);
private slots:
    void updatePlot();
    void serieCheckable(bool);
    void startPlotting(bool);

private:
    void createToolBox();
    void createSerieBox();
    void createValueLabel();
    void updateNodeCombox(QString, QString, QString, QString);
    void updateSeriesComboBox(int);
    void seriesTextChange(QString);
    qreal getYvalue(QString, QString);
    // Main layout
    QVBoxLayout *verticalLayout;

    // tool box
    QFrame *toolFrame;
    QLabel *labels[2];
    // select node-id and data type
    QComboBox *uavcanInfoBox[2];

    // select variable to read
    QLabel *dataLabels[3];
    QComboBox *seriesBox[3];
    QCheckBox *seriesCheckbox[3];
    QPushButton *button;

    QChart *chart;
    QChartView *chartView;
    QLineSeries *series[3];
    QValueAxis *xAxis;
    QValueAxis *yAxis;

    // current value readed
    QLabel *current_value[3];

    QTimer *timer;
    int xValue;
    qreal yMax = 1.0;
    qreal yMin = -1.0;

    // node_id : dt : protocol
    QMap<QString, QMap<QString, QString>> nodeLookupTable;

    // node_id : dt_id : payload
    QMap<QString, QMap<QString, QString>> payloadLookupTable;

    // uavcan decode
    UavcanDecoder *uavcan;

};

#endif // PLOTTERWIDGET_H
