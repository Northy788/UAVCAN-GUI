#include "gnsswidget.h"

GNSSWidget::GNSSWidget(QWidget *parent) :
    QWidget(parent),
    latitude_deg(13.7563),
    longitude_deg(100.5018)
{

    // Display location names and coordinates
    setFixedSize(1024, 600);
    qDebug() << "GNSS open";
    uavcan = new UavcanDecoder;
    layout = new QHBoxLayout(this);
    QFrame *centralFrame = new QFrame;
    centralFrame->setObjectName("centralFrame");
    hBox = new QHBoxLayout;
    hBox->setContentsMargins(5,5,0,0);
    createStatusView();

    mapView = new QQuickWidget(this);
    mapView->setSource(QUrl(QStringLiteral("qrc:/map_view.qml")));
    mapView->setResizeMode(QQuickWidget::SizeRootObjectToView);

    mapView->rootContext()->setContextProperty("gnssWindow", this);
    hBox->addWidget(mapView);
    centralFrame->setLayout(hBox);
    layout->addWidget(centralFrame);
    setLayout(layout);

    // set Style sheet
    QFile fileStyle(":/style.qss");
    if(!fileStyle.open(QFile::ReadOnly))
        qDebug() << "can't read file";
    QString styleString = fileStyle.readAll();
    fileStyle.close();
    setStyleSheet(styleString);
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
        if (nodeLookuptable[node_id].isEmpty())
        {
            nodeLookuptable[node_id] = uavcan->getProtocolType(protocol);
            nodeBox->addItem(node_id);
            node_protocol->setText(nodeLookuptable[node_id]);
        }
        payloadLookuptable[node_id] = data;
        updateStatus();
    }
}

void GNSSWidget::updateStatus()
{
    if (nodeLookuptable.isEmpty())
        return ;
    if (payloadLookuptable[nodeBox->currentText()].isEmpty())
        return ;

    QString node_id = nodeBox->currentText();
    // time gnss_time gnss_timestandard numleap

    QVector<QString> payload = payloadLookuptable[node_id].split(' ');

    timestamp = QTime::fromMSecsSinceStartOfDay(payload[0].toLong() / 1000000000).toString(); // uptime of GNSS

    GNSS_timestamp = payload[1].toLong(); // current time

    // if time standard is UTC
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(GNSS_timestamp / 1000);
    dateTime.setTimeSpec(Qt::LocalTime);
    QString formattedTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");

    timestandard = payload[2].toInt(); // time standard
    // payload[3] is numleap
    gnss_longtitude = payload[4].toDouble(); // lontitude value
    gnss_latitude = payload[5].toDouble(); // latitude value
    // payload[6]

    // Display protocol of node id
    node_protocol->setText(nodeLookuptable[node_id]);

    // Display that node has gnss variable?
    gnss_label->setText("GNSS: Online");

    // show timestamp of gnss
    QTableWidgetItem *uptimeItem = new QTableWidgetItem(timestamp);
    table->setItem(1, 1, uptimeItem);
    table->item(1,1)->setFlags(table->item(1, 0)->flags() & ~Qt::ItemIsEditable);

    // show current time
    QTableWidgetItem *standardItem = new QTableWidgetItem("Null");
    QTableWidgetItem *gnssTimeItem = new QTableWidgetItem("Null");
    switch (timestandard) {
    case 0: // None
        standardItem->setText("None");
        gnssTimeItem->setText(QString::number(GNSS_timestamp));
        break;
    case 1: // TAI
        standardItem->setText("TAI");
        gnssTimeItem->setText("NaN");
        break;
    case 2: // UTC
        standardItem->setText("UTC");
        gnssTimeItem->setText(formattedTime);
        break;
    case 3: // GPS
        standardItem->setText("GPS");
        gnssTimeItem->setText("NaN");
        break;
    }
    table->setItem(2, 1, standardItem);
    table->item(2, 1)->setFlags(table->item(2, 1)->flags() & ~Qt::ItemIsEditable);
    table->setItem(3, 1, gnssTimeItem);
    table->item(3, 1)->setFlags(table->item(3, 1)->flags() & ~Qt::ItemIsEditable);

    // display longtitude value
    QTableWidgetItem *longtitudeItem = new QTableWidgetItem(QString::number(gnss_longtitude, 'f', 4));
    table->setItem(4, 1, longtitudeItem);
    table->item(4, 1)->setFlags(table->item(4, 1)->flags() & ~Qt::ItemIsEditable);

    // dislay latitude value
    QTableWidgetItem *latitudeItem = new QTableWidgetItem(QString::number(gnss_latitude, 'f', 4));
    table->setItem(5, 1, latitudeItem);
    table->item(5, 1)->setFlags(table->item(5, 1)->flags() & ~Qt::ItemIsEditable);

    setLatitude(gnss_latitude);
    setLongitude(gnss_longtitude);
}

void GNSSWidget::createStatusView()
{
    QHBoxLayout *nodeLayout = new QHBoxLayout;
    nodeBox = new QComboBox;
    node_protocol = new QLabel(": Unknow");
    gnss_label = new QLabel("GNSS: Offline");
    nodeLayout->addWidget(nodeBox);
    nodeLayout->addWidget(node_protocol);
    qDebug() << "GNSS init1";
    table = new QTableWidget;
    table->setColumnCount(2);
    table->setRowCount(6);
    table->setColumnWidth(0, 150);
    table->setColumnWidth(1, 200);
    table->setFixedWidth(350);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setVisible(false);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QTableWidgetItem *nodeItem = new QTableWidgetItem("Status");
    table->setItem(0, 0, nodeItem);
    table->item(0, 0)->setFlags(table->item(0, 0)->flags() & ~Qt::ItemIsEditable);
    qDebug() << "GNSS init2";

    QTableWidgetItem *uptimeItem = new QTableWidgetItem("Timestamp");
    table->setItem(1, 0, uptimeItem);
    table->item(1,0)->setFlags(table->item(1, 0)->flags() & ~Qt::ItemIsEditable);
    qDebug() << "GNSS init3";

    QTableWidgetItem *standardItem = new QTableWidgetItem("Time standard");
    table->setItem(2, 0, standardItem);
    table->item(2, 0)->setFlags(table->item(2, 0)->flags() & ~Qt::ItemIsEditable);
    qDebug() << "GNSS init4";

    QTableWidgetItem *GnssTimeItem = new QTableWidgetItem("GNSS Time");
    table->setItem(3, 0, GnssTimeItem);
    table->item(3, 0)->setFlags(table->item(3, 0)->flags() & ~Qt::ItemIsEditable);
    qDebug() << "GNSS init5";

    QTableWidgetItem *longtitudeItem = new QTableWidgetItem("Longtitude");
    table->setItem(4, 0, longtitudeItem);
    table->item(4, 0)->setFlags(table->item(4, 0)->flags() & ~Qt::ItemIsEditable);
    qDebug() << "GNSS init6";

    QTableWidgetItem *latitudeItem = new QTableWidgetItem("Latitude");
    table->setItem(5, 0, latitudeItem);
    table->item(5, 0)->setFlags(table->item(5, 0)->flags() & ~Qt::ItemIsEditable);
    qDebug() << "GNSS init6";

    table->hideRow(0);
    qDebug() << "GNSS init7";

    connect(nodeBox, &QComboBox::currentTextChanged, this, &GNSSWidget::nodeChanged);
    QVBoxLayout *vBox = new QVBoxLayout;
    qDebug() << "GNSS init71";
    vBox->addLayout(nodeLayout);
    qDebug() << "GNSS init72";
    vBox->addWidget(gnss_label);
    qDebug() << "GNSS init73";
    vBox->addWidget(table);
    qDebug() << "GNSS init74";
    vBox->setContentsMargins(0 , 0, 0 ,0);
    qDebug() << "GNSS init75";
    vBox->setSpacing(5);
    qDebug() << "GNSS init";
    hBox->addLayout(vBox);
}

double GNSSWidget::latitude() const
{
    return latitude_deg;
}

void GNSSWidget::setLatitude(double latitude)
{
    if (qFuzzyCompare(latitude_deg, latitude))
        return;

    latitude_deg = latitude;
    emit latitudeChanged();
}

double GNSSWidget::longitude() const
{
    return longitude_deg;
}

void GNSSWidget::setLongitude(double longitude)
{
    if (qFuzzyCompare(longitude_deg, longitude))
        return;

    longitude_deg = longitude;
    emit longitudeChanged();
}

void GNSSWidget::nodeChanged(QString text)
{
    updateStatus();
}

