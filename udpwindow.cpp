#include "udpwindow.h"

UdpWindow::UdpWindow(QWidget *parent) : QWidget(parent)
{
    verticalLayout = new QVBoxLayout;
    networkInterface = new NetworkInterface();
    createToolBox();
    createTableBox();
    createDetailBox();

    update_interface();
    this->setLayout(verticalLayout);
}

void UdpWindow::createToolBox()
{
    QHBoxLayout *horizontralLayout = new QHBoxLayout;
    toolFrame = new QFrame;

    QFrame *groupFrame = new QFrame;
    QHBoxLayout *groupLayout = new QHBoxLayout;
    labels[0] = new QLabel("select interface: ");
    QHBoxLayout *hBox0 = new QHBoxLayout;
    interfaceBox = new QComboBox;
    hBox0->addWidget(labels[0]);
    hBox0->addWidget(interfaceBox);

    labels[1] = new QLabel("select port: ");
    QHBoxLayout *hBox1 = new QHBoxLayout;
    portSpinBox = new QSpinBox;
    portSpinBox->setMinimum(0);
    portSpinBox->setMaximum(65535);
    hBox1->addWidget(labels[1]);
    hBox1->addWidget(portSpinBox);
    groupLayout->addLayout(hBox0);
    groupLayout->addLayout(hBox1);
    groupLayout->setSpacing(5);
    groupLayout->setAlignment(Qt::AlignLeft);

    button[0] = new QPushButton("Start");
    button[0]->setCheckable(true);
    button[1] = new QPushButton("Refresh");
    groupLayout->addWidget(button[1]);
    groupLayout->addWidget(button[0]);
    groupFrame->setLayout(groupLayout);

    connect(button[0], &QPushButton::toggled, this, &UdpWindow::connected);

    horizontralLayout->addWidget(groupFrame);
    horizontralLayout->setStretchFactor(groupFrame, 0);

    toolFrame->setLayout(horizontralLayout);

    verticalLayout->addWidget(toolFrame);
    verticalLayout->setStretchFactor(toolFrame, 0);
}
void UdpWindow::createTableBox()
{
    tableFrame = new QFrame;

    udpTable = new QTableWidget;

    // Set the number of columns (Time, Source, Destination, Payload)
    udpTable->setColumnCount(4);
    QStringList headerLabels;
    headerLabels << "Time" << "Source" << "Destination" << "Payload";
    udpTable->setHorizontalHeaderLabels(headerLabels);
    udpTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Set fixed column widths for Time, Source, and Destination
    udpTable->setColumnWidth(0, 120); // Time (adjust width as needed)
    udpTable->setColumnWidth(1, 150); // Source (adjust width as needed)
    udpTable->setColumnWidth(2, 150); // Destination (adjust width as needed)


    // Set horizontal header stretch for even distribution of remaining space
    udpTable->horizontalHeader()->setStretchLastSection(true);
    udpTable->verticalHeader()->setVisible(false);

    // Set horizontal header stretch for even distribution of remaining space
    udpTable->horizontalHeader()->setStretchLastSection(true);
    udpTable->verticalHeader()->setVisible(false);

    // Set the table to grow and shrink with the window
    udpTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(networkInterface, &NetworkInterface::hasReadyReadDatagram, this, &UdpWindow::updateTable);

    verticalLayout->addWidget(udpTable);
    verticalLayout->setStretchFactor(udpTable, 3);
}

void UdpWindow::createDetailBox()
{
    QHBoxLayout *hBox = new QHBoxLayout;
    detailsList = new QListWidget();
    payloadDetail = new QTextEdit;
    payloadDetail->setDisabled(true);
    hBox->addWidget(detailsList);
    hBox->addWidget(payloadDetail);
    hBox->setStretchFactor(detailsList, 3);
    hBox->setStretchFactor(payloadDetail, 2);

    connect(udpTable, &QTableWidget::cellPressed, this, &UdpWindow::onSelectTable);
    verticalLayout->addLayout(hBox);
}

void UdpWindow::update_interface()
{
    interfaceBox->clear();
    networkInterface->getNetworkInterfaceList();
    QList<QString> name, ipv4;
    name = networkInterface->getAVailableInterface();
    ipv4 = networkInterface->getAvailableIPv4();
    for(int i = 0; i < name.count(); i++)
    {
        interfaceBox->addItem(name.at(i) + ":" + ipv4.at(i));
    }
}

void UdpWindow::updateTable()
{
    while (networkInterface->queue.size())
    {
        QPair<qint64,QNetworkDatagram> data = networkInterface->queue.dequeue();
        QNetworkDatagram datagram = data.second;
        qint64 timestamp = data.first;
        appendUdpTable(timestamp, datagram);
    }
}

void UdpWindow::appendUdpTable(qint64 timestamp, QNetworkDatagram datagram)
{
    QString time = QString::number(timestamp / 1000.0, 'f', 3); // Get timestamp in milliseconds (string format)
    QString source = datagram.senderAddress().toString();
    QString destination = datagram.destinationAddress().toString();
    QString payload = QString(datagram.data());
    int row = udpTable->rowCount();
    udpTable->insertRow(row);

    // Set the data for each column in the row
    QTableWidgetItem *timeItem = new QTableWidgetItem(time);
    udpTable->setItem(row, 0, timeItem);
    udpTable->item(row, 0)->setFlags(udpTable->item(row, 0)->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *sourceItem = new QTableWidgetItem(source);
    udpTable->setItem(row, 1, sourceItem);
    udpTable->item(row, 1)->setFlags(udpTable->item(row, 1)->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *destinationItem = new QTableWidgetItem(destination);
    udpTable->setItem(row, 2, destinationItem);
    udpTable->item(row, 2)->setFlags(udpTable->item(row, 2)->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *payloadItem = new QTableWidgetItem(payload);
    udpTable->setItem(row, 3, payloadItem);
    udpTable->item(row, 3)->setFlags(udpTable->item(row, 3)->flags() & ~Qt::ItemIsEditable);

    sourcePorts.append(QString::number(datagram.senderPort()));
    destinationPorts.append(QString::number(datagram.destinationPort()));
    udpTable->scrollToBottom();
    emit udpReceived(payload);
}

void UdpWindow::connected(bool isConnected)
{
    if (isConnected)
    {
        udpTable->clearContents();
        udpTable->setRowCount(0);
        int inf = interfaceBox->currentIndex();
        QString host_addr = networkInterface->getAvailableIPv4().at(inf);
        int host_port = portSpinBox->value();
        networkInterface->setUdpSocket(host_addr, host_port);
        networkInterface->startReceiveDatagram();
    }
    else
    {
        networkInterface->stopReceiveDatagram();
    }
}

void UdpWindow::onSelectTable(int row, int col)
{
    detailsList->clear();
    payloadDetail->clear();
    // Get items from the selected table row
    QString timeItemText = udpTable->item(row, 0)->text();
    QString sourceItemText = udpTable->item(row, 1)->text();
    QString destinationItemText = udpTable->item(row, 2)->text();
    QString payloadItemText = udpTable->item(row, 3)->text();

    qreal prev_time;
    switch (row) {
    case 0:
        prev_time = 0.0;
        break;
    default:
        prev_time = udpTable->item(row, 0)->text().toDouble() - udpTable->item(row - 1, 0)->text().toDouble();
        break;
    }
    QString details = QString("Timestamp: %1 s\n Time since first frame: %2 s\n Time since previous frame: %3 s")
                          .arg(timeItemText)
                          .arg(timeItemText)
                          .arg(QString::number(prev_time, 'f', 3));
    detailsList->addItem(details);
    details = QString("Source IPv4: %1\nSource Port: %2")
                  .arg(sourceItemText)
                  .arg(sourcePorts.at(row));
    detailsList->addItem(details);
    details = QString("Destination IPv4: %1\nDestination Port: %2")
                  .arg(destinationItemText)
                  .arg(destinationPorts.at(row));
    detailsList->addItem(details);
    details = QString("Payload: %1 (%2 Bytes)")
                  .arg(payloadItemText)
                  .arg(QString::number(payloadItemText.length()));
    detailsList->addItem(details);

    payloadDetail->setPlainText(payloadItemText);
}
