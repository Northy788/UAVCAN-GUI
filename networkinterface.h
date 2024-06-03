#ifndef NETWORKINTERFACE_H
#define NETWORKINTERFACE_H

#include <QObject>
#include <QNetworkInterface>
#include <QNetworkDatagram>
#include <QUdpSocket>
#include <QQueue>
#include <QElapsedTimer>

class NetworkInterface : public QObject
{
    Q_OBJECT
public:
    explicit NetworkInterface(QObject *parent = nullptr);

    QQueue<QPair<qint64,QNetworkDatagram>> queue;

    // Network Interface
    void getNetworkInterfaceList(void);
    QList<QString> getAVailableInterface(void);
    QList<QString> getAvailableIPv4(void);

    // UDP Socket
    void setUdpSocket(QString, int);
    void startReceiveDatagram(void);
    void stopReceiveDatagram(void);
    void writeDatagram(QByteArray);

signals:
    /*
     * when have incomoning datagram store in buffer
     * it will colleted in fifo, so that if you need to read
     * datagram you need to read it by dequque the fifo
    */
    void hasReadyReadDatagram();

private slots:
    // UDP Socket
    void readPendingDatagram(void);

private:


    QUdpSocket *udpSocket;

    QList<QString> interface_name;
    QList<QString> ipv4;

    QElapsedTimer timer;
};

#endif // NETWORKINTERFACE_H
