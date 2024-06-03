#ifndef GNSSWIDGET_H
#define GNSSWIDGET_H

#include <QWidget>
#include <QMap>
#include <QGeoCoordinate>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QQuickWidget>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QLabel>
#include <QComboBox>

#include "uavcandecoder.h"

class GNSSWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GNSSWidget(QWidget *parent = nullptr);

public slots:
    void payloadReceived(QString);

signals:


private slots:
    void updateStatus(QString);

private:
    void init();
    void createStatusView();

    QHBoxLayout *layout;

    QMap<QString, QGeoCoordinate> locations;
    QMap<QString, QString> payloadCahce;
    QQuickWidget *mapView;

    // DroneCAN
    //gnss.fix2
    qint64 timestamp = 0;
    qint64 GNSS_timestamp = 0;
    QMap<QString, QString> GNSS_time_standard;
    QMap<QString, QString> GNSS_status;
    QMap<QString, QString> GNSS_mode;
    QString timestandard = "None";
    // QString status;
    // QString mode;
    // qint64 num_leap_second = 0;
    qint64 longitude_deg = 10050180000;
    qint64 latitude_deg = 1375630000;
    // qint64 height_ellipsoid_mm;
    // qint64 height_msl_mm;
    // qreal ned_velocity[3];


    UavcanDecoder *uavcan;
    QComboBox *nodeBox;
    QLabel *node_protocol;
    QLabel *gnss_label;
    QLabel *timestandard_label;
    QLabel *timestamp_label;
    QLabel *GNSS_timestamp_label;
    QLabel *latitude_label;
    QLabel *longtitude_label;
};

#endif // GNSSWIDGET_H
