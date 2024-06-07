#ifndef GNSSWIDGET_H
#define GNSSWIDGET_H

#include <QWidget>
#include <QObject>
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
#include <QSpinBox>
#include <QTextEdit>
#include <QVariant>
#include <qqml.h>
#include <QQmlContext>
#include <QTime>
#include <QTimeZone>
#include <QTableWidget>
#include <QHeaderView>
#include <QStyle>
#include <QTableWidgetItem>


#include "uavcandecoder.h"

class GNSSWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double latitude READ latitude WRITE setLatitude NOTIFY latitudeChanged)
    Q_PROPERTY(double longitude READ longitude WRITE setLongitude NOTIFY longitudeChanged)


public:
    explicit GNSSWidget(QWidget *parent = nullptr);


    enum Timestandard{
        NONE,
        TAI,
        UTC,
        GPS
    };

    enum GnssStatus{
        NO_FIX,
        TIME_ONLY,
        TwoDimention_FIX,
        ThreeDimention_FIX
    };

    enum GnssMode{
        SINGL,
        DGPS,
        RTK,
        PPP
    };

    double latitude() const;
    void setLatitude(double);

    double longitude() const;
    void setLongitude(double);


public slots:
    void payloadReceived(QString);
    void nodeChanged(QString);

signals:
    void latitudeChanged();
    void longitudeChanged();

private slots:
    void updateStatus();


protected:

private:
    void createStatusView();

    QHBoxLayout *layout;
    QHBoxLayout *hBox;
    QMap<QString, QGeoCoordinate> locations;
    QMap<QString, QString> nodeLookuptable;
    QMap<QString, QString> payloadLookuptable;
    QQuickWidget *mapView;
    QQmlContext *context;

    // DroneCAN
    //gnss.fix2
    QString timestamp;
    qint64 GNSS_timestamp = 0;
    int timestandard = 0;
    // QString status;
    // QString mode;
    // qint64 num_leap_second = 0;
    qreal longitude_deg;
    qreal latitude_deg;

    qreal gnss_latitude;
    qreal gnss_longtitude;
    // qint64 height_ellipsoid_mm;
    // qint64 height_msl_mm;
    // qreal ned_velocity[3];


    UavcanDecoder *uavcan;
    QComboBox *nodeBox;
    QTableWidget *table;
    QLabel *node_protocol;
    QLabel *gnss_label;
    QLabel *timestandard_label;
    QLabel *timestamp_label;
    QLabel *GNSS_timestamp_label;
    QLabel *latitude_label;
    QLabel *longtitude_label;
};

#endif // GNSSWIDGET_H
