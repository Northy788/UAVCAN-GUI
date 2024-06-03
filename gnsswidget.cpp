#include "gnsswidget.h"

GNSSWidget::GNSSWidget(QWidget *parent) : QWidget(parent) {

    // Display location names and coordinates
    setFixedSize(1024, 600);
    init();
    layout = new QHBoxLayout(this);
    createStatusView();

    mapView = new QQuickWidget(this);
    mapView->setSource(QUrl("qrc:/map_view.qml"));
    mapView->show();
    layout->addWidget(mapView);
    layout->setStretchFactor(mapView, 5);
    setLayout(layout);
}

void GNSSWidget::payloadReceived(QString payload)
{
    QVector<QString> payloadSplited = payload.split(' ');
    QString protocol = payloadSplited.at(0);
    QString dtid = payloadSplited.at(1);
    QString node_id = payloadSplited.at(2);
    QString data = payloadSplited.mid(3).join(' ');
    if (uavcan->getProtocolType(protocol) == "DroneCAN" && uavcan->getDataType(dtid) == "uavcan.equipment.gnss.Fix2")
    {
        if (payloadCahce[node_id].isEmpty())
            payloadCahce[node_id] = uavcan->getProtocolType(protocol);
        updateStatus(data);
    }
}

void GNSSWidget::updateStatus(QString data)
{
    QVector<QString> payload = data.split(' ');
    timestamp = payload[0].toLongLong();
    GNSS_timestamp = payload[1].toLongLong();
    timestandard = GNSS_time_standard[payload[2]];
    longitude_deg = payload[5].toLongLong();
    latitude_deg = payload[6].toLongLong();
}

void GNSSWidget::createStatusView()
{
    QHBoxLayout *nodeLayout = new QHBoxLayout;
    nodeBox = new QComboBox;
    node_protocol = new QLabel(": Unknow");
    nodeLayout->addWidget(nodeBox);
    nodeLayout->addWidget(node_protocol);

    QVBoxLayout *vBox = new QVBoxLayout;
    gnss_label = new QLabel("GNSS : Unavailable");
    timestandard_label = new QLabel("Time Standard: " + timestandard);
    timestamp_label = new QLabel("Timestamp: " + QString::number(timestamp));
    GNSS_timestamp_label = new QLabel("GNSS Timestamp: " + QString::number(GNSS_timestamp));
    latitude_label = new QLabel("Latitude: " + QString::number(latitude_deg / 100000000.0, 'f', 4));
    longtitude_label = new QLabel("Longtitude: " + QString::number(longitude_deg / 100000000.0, 'f', 4));
    vBox->addLayout(nodeLayout);
    vBox->addWidget(gnss_label);
    vBox->addWidget(timestandard_label);
    vBox->addWidget(timestamp_label);
    vBox->addWidget(GNSS_timestamp_label);
    vBox->addWidget(latitude_label);
    vBox->addWidget(longtitude_label);

    vBox->setContentsMargins(0 , 0, 0 ,0);
    vBox->setSpacing(5);
    vBox->setAlignment(Qt::AlignTop);

    layout->addLayout(vBox);
    layout->setStretchFactor(vBox, 2);
}

void GNSSWidget::init()
{
    GNSS_time_standard["0"] = "NONE";
    GNSS_time_standard["1"] = "TAI";
    GNSS_time_standard["2"] = "UTC";
    GNSS_time_standard["3"] = "GPS";

    GNSS_status["0"] = "No Fix";
    GNSS_status["1"] = "Time Only";
    GNSS_status["2"] = "2D Fix";
    GNSS_status["3"] = "3D Fix";

    GNSS_mode["0"] = "Single";
    GNSS_mode["0"] = "DGPS";
    GNSS_mode["0"] = "RTK";
    GNSS_mode["0"] = "PPP";
}
