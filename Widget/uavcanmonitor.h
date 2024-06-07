#ifndef UAVCANMONITOR_H
#define UAVCANMONITOR_H

#include <QWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMap>
#include <QDateTime>
#include <QTime>

#include "../Uavcan/uavcandecoder.h"

class UavcanMonitor : public QWidget
{
    Q_OBJECT
public:
    explicit UavcanMonitor(QWidget *parent = nullptr);

signals:

public slots:
    void payloadReceived(QString);

private slots:
    void getSelectedRow(int, int);
private:
    void createNodeTable();
    void createTransferTable();
    void createPayloadDetail();
    void onlineNodeUpdate(QString, QString, QString, QString);
    void appendNodeTable(QString, QString, QString, QString, QString);
    void transfetDataUpdate(QString, QString, QString, QString, QString);
    void appendTransferTable(QString, QString, QString, QString);
    void updateDetail(int);
    QHBoxLayout *horizontralLayout;

    QVBoxLayout *verticalLayout;

    // online node table
    QGroupBox *nodeGroup;
    QTableWidget *nodeTable;

    // node_id : bool
    QMap<QString, int> nodeLookupTable;
    // data transfer table
    QGroupBox *transferGroup;
    QTableWidget *transferTable;

    // node_id : dt_id : row
    QMap<QString, QMap<QString, qint64>> tfRowLookupTable;

    // node_id : dt_id : payload
    QMap<QString, QMap<QString, QString>> tfPayloadLookupTable;

    // dt_name : variable_name : type : sign : count
    QMap<QString, QMap<QString, QMap<QString, QMap<QString, int>>>> fieldLookuptable;


    // payload details
    QGroupBox *payloadGroup;
    QListWidget *payloadDetails;
    int currentRow = 0;

    // decoder
    UavcanDecoder *uavcan;
};

#endif // UAVCANMONITOR_H
