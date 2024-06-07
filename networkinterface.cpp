#include "networkinterface.h"

NetworkInterface::NetworkInterface(QObject *parent) : QObject(parent)
{
    // get valid interface <name, ipv4>
    getNetworkInterfaceList();

    // init udpsocket
    udpSocket = new QUdpSocket(this);


    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagram()));
}

void NetworkInterface::getNetworkInterfaceList(void)
{
    interface_name.clear();
    ipv4.clear();
    // Get all network interfaces
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    // Loop through each interface
    for (const QNetworkInterface& iface : interfaces) {
        // Check if the interface is active (up and running)
        if (iface.isValid()) {
            // Get the human-readable name of the interface
            QString interfaceName = iface.humanReadableName();

            // Get a list of addresses for this interface
            QList<QNetworkAddressEntry> addressEntries = iface.addressEntries();

            // Loop through each address entry
            for (const QNetworkAddressEntry& entry : addressEntries) {
                // Check for IPv4 address
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    ipv4.append(entry.ip().toString());
                    interface_name.append(interfaceName);
                    break; // Get only the first IPv4 address (optional)
                }
            }
        }
    }
}

QList<QString> NetworkInterface::getAVailableInterface()
{
    return interface_name;
}

QList<QString> NetworkInterface::getAvailableIPv4()
{
    return ipv4;
}

void NetworkInterface::setUdpSocket(QString ipv4, int port)
{
    udpSocket->bind(QHostAddress(ipv4), port);
    if (udpSocket->state() == QAbstractSocket::BoundState)
    {
        qDebug() << "Udp Socket Activated";
        qDebug() << "IP Address:" << udpSocket->localAddress();
        qDebug() << "Port:" << udpSocket->localPort();
    }
    else
        qDebug() << "preppare failed.";
}

void NetworkInterface::readPendingDatagram()
{
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        //FIFO->rxPush(datagram);
        if (queue.size() < 128)
            queue.enqueue(qMakePair(timer.elapsed(), datagram));
        else
            qDebug() << "fifo full";
    }
    emit hasReadyReadDatagram();
}

void NetworkInterface::startReceiveDatagram()
{
    connect(udpSocket, &QUdpSocket::readyRead, this, &NetworkInterface::readPendingDatagram);
    qDebug() << udpSocket->state() << "(ready to read)";
    // FIFO->timestampStart();
    timer.restart();
}

void NetworkInterface::stopReceiveDatagram()
{
    disconnect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagram()));
    udpSocket->close();
    qDebug() << udpSocket->state() << "(disconnect signal-slot and close socket)";
}
void NetworkInterface::writeDatagram(QByteArray payload)
{
    udpSocket->writeDatagram(payload, udpSocket->localAddress(), udpSocket->localPort());
}
