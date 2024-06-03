#ifndef UDPWINDOW_H
#define UDPWINDOW_H

#include <QWidget>
#include <QObject>
#include <QLayout>
#include <QFrame>
#include <QTableWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QLine>
#include <QListWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QVector>

#include "networkinterface.h"

class UdpWindow : public QWidget
{
    Q_OBJECT
public:
    explicit UdpWindow(QWidget *parent = nullptr);

signals:
    void udpReceived(QString);

private slots:
    void updateTable();
    void onSelectTable(int, int);
    void connected(bool);

private:
    void createToolBox();
    void createTableBox();
    void createDetailBox();
    void update_interface();
    void appendUdpTable(qint64, QNetworkDatagram);

    // Main layout
    QVBoxLayout *verticalLayout;

    // Tools
    QFrame *toolFrame;
    QLabel *labels[2];
    QComboBox *interfaceBox;
    QSpinBox *portSpinBox;
    QPushButton *button[2];

    // Udp Table
    QFrame *tableFrame;
    QTableWidget *udpTable;
    QVector<QString> sourcePorts;
    QVector<QString> destinationPorts;

    // Display Details of datagram selected
    QListWidget *detailsList;
    QTextEdit *payloadDetail;

    // Network Interfdace
    NetworkInterface *networkInterface;

};

#endif // UDPWINDOW_H
