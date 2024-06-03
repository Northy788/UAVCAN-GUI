#include "uavcanmonitor.h"

UavcanMonitor::UavcanMonitor(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(1024, 600);
    horizontralLayout = new QHBoxLayout;
    verticalLayout = new QVBoxLayout;

    createNodeTable();
    createPayloadDetail();
    horizontralLayout->addLayout(verticalLayout);
    createTransferTable();
    horizontralLayout->setStretchFactor(verticalLayout, 1);
    horizontralLayout->setStretchFactor(transferGroup, 1);
    setLayout(horizontralLayout);

    uavcan = new UavcanDecoder;
}

void UavcanMonitor::createNodeTable()
{

    nodeTable = new QTableWidget;
    nodeGroup = new QGroupBox("Online nodes");
    QVBoxLayout *vBox = new QVBoxLayout;
    nodeGroup->setLayout(vBox);
    // Set the number of columns (Node-Id, Uptime, Health, Mode, Vender-specific)
    nodeTable->setColumnCount(5);
    QStringList headerLabels;
    headerLabels << "Node ID" << "Uptime" << "Health" << "Mode" << "Vender-specific";
    nodeTable->setHorizontalHeaderLabels(headerLabels);
    nodeTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Set fixed column widths for Node-ID, Uptime, Health, and Mode
    nodeTable->setColumnWidth(0, 50); // Node-Id (adjust width as needed)
    nodeTable->setColumnWidth(1, 70); // Uptime (adjust width as needed)
    nodeTable->setColumnWidth(2, 80); // Health (adjust width as needed)
    nodeTable->setColumnWidth(3, 120); // Mode (adjust width as needed)


    // Set horizontal header stretch for even distribution of remaining space
    nodeTable->horizontalHeader()->setStretchLastSection(true);
    nodeTable->verticalHeader()->setVisible(false);

    // Set horizontal header stretch for even distribution of remaining space
    nodeTable->horizontalHeader()->setStretchLastSection(true);
    nodeTable->verticalHeader()->setVisible(false);

    // Set the table to grow and shrink with the window
    nodeTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    vBox->addWidget(nodeTable);
    verticalLayout->addWidget(nodeGroup);
}

void UavcanMonitor::createTransferTable()
{
    transferTable = new QTableWidget;
    transferGroup = new QGroupBox("Message transfer");
    QVBoxLayout *vBox = new QVBoxLayout;
    transferGroup->setLayout(vBox);
    // Set the number of columns (Node-Id, Uptime, Health, Mode, Vender)
    transferTable->setColumnCount(4);
    QStringList headerLabels;
    headerLabels << "Src" << "Dst" << "Protocol" << "Data type";
    transferTable->setHorizontalHeaderLabels(headerLabels);
    transferTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Set fixed column widths for Node-ID, Uptime, Health, and Mode
    transferTable->setColumnWidth(0, 50); // Node-Id (adjust width as needed)
    transferTable->setColumnWidth(1, 50); // Uptime (adjust width as needed)
    transferTable->setColumnWidth(2, 80); // Health (adjust width as needed)


    // Set horizontal header stretch for even distribution of remaining space
    transferTable->horizontalHeader()->setStretchLastSection(true);
    transferTable->verticalHeader()->setVisible(false);

    // Set horizontal header stretch for even distribution of remaining space
    transferTable->horizontalHeader()->setStretchLastSection(true);
    transferTable->verticalHeader()->setVisible(false);

    // Set the table to grow and shrink with the window
    transferTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    vBox->addWidget(transferTable);
    horizontralLayout->addWidget(transferGroup);

    connect(transferTable, &QTableWidget::cellPressed, this, &UavcanMonitor::getSelectedRow);
}

void UavcanMonitor::createPayloadDetail()
{
    payloadGroup = new QGroupBox("Payload Details");
    QVBoxLayout *vBox = new QVBoxLayout;
    payloadGroup->setLayout(vBox);
    payloadDetails = new QListWidget;

    vBox->addWidget(payloadDetails);
    verticalLayout->addWidget(payloadGroup);
}

void UavcanMonitor::payloadReceived(QString payload)
{
    QVector<QString> payloadSplited = payload.split(' ');
    QString protocol = payloadSplited.at(0);
    QString dtid = payloadSplited.at(1);
    QString node_id = payloadSplited.at(2);
    QString data = payloadSplited.mid(3).join(' ');
    onlineNodeUpdate(node_id, protocol, dtid, data);
    transfetDataUpdate(node_id, " ", protocol, dtid, data);
}

void UavcanMonitor::appendNodeTable(QString node_id, QString uptime, QString healt, QString mode, QString vendor)
{
    int row = nodeTable->rowCount();
    if (!nodeLookupTable.contains(node_id))
    {
        nodeTable->insertRow(row);
        nodeLookupTable[node_id] = row;
    }
    else
    {
        row = nodeLookupTable[node_id];
    }
    // Set the data for each column in the row
    QTableWidgetItem *nodeItem = new QTableWidgetItem(node_id);
    nodeTable->setItem(row, 0, nodeItem);
    nodeTable->item(row, 0)->setFlags(nodeTable->item(row, 0)->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *uptimeItem = new QTableWidgetItem(uptime);
    nodeTable->setItem(row, 1, uptimeItem);
    nodeTable->item(row, 1)->setFlags(nodeTable->item(row, 1)->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *healthItem = new QTableWidgetItem(healt);
    nodeTable->setItem(row, 2, healthItem);
    nodeTable->item(row, 2)->setFlags(nodeTable->item(row, 2)->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *modeItem = new QTableWidgetItem(mode);
    nodeTable->setItem(row, 3, modeItem);
    nodeTable->item(row, 3)->setFlags(nodeTable->item(row, 3)->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *vendorItem = new QTableWidgetItem(vendor);
    nodeTable->setItem(row, 4, vendorItem);
    nodeTable->item(row, 4)->setFlags(nodeTable->item(row, 4)->flags() & ~Qt::ItemIsEditable);
    // nodeTable->scrollToBottom();
}

void UavcanMonitor::appendTransferTable(QString src, QString dst, QString protocl, QString dt)
{
    int row = transferTable->rowCount();
    if (row == 0)
    {
        tfRowLookupTable[src][uavcan->getDataType(dt)] = row;
        transferTable->insertRow(row);
    }
    else
    {
        // qDebug() << tfRowLookupTable.contains(src);
        if (tfRowLookupTable[src].keys().contains(uavcan->getDataType(dt)))
        {
            row = tfRowLookupTable[src][uavcan->getDataType(dt)];
        }
        else
        {
            tfRowLookupTable[src][uavcan->getDataType(dt)] = row;
            transferTable->insertRow(row);
        }
    }
    // Set the data for each column in the row
    QTableWidgetItem *srcItem = new QTableWidgetItem(src);
    transferTable->setItem(row, 0, srcItem);
    transferTable->item(row, 0)->setFlags(transferTable->item(row, 0)->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *dstItem = new QTableWidgetItem(dst);
    transferTable->setItem(row, 1, dstItem);
    transferTable->item(row, 1)->setFlags(transferTable->item(row, 1)->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *protocolItem = new QTableWidgetItem(uavcan->getProtocolType(protocl));
    transferTable->setItem(row, 2, protocolItem);
    transferTable->item(row, 2)->setFlags(transferTable->item(row, 2)->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *dtItem = new QTableWidgetItem(uavcan->getDataType(dt));
    transferTable->setItem(row, 3, dtItem);
    transferTable->item(row, 3)->setFlags(transferTable->item(row, 3)->flags() & ~Qt::ItemIsEditable);
    // nodeTable->scrollToBottom();
}

void UavcanMonitor::onlineNodeUpdate(QString node_id, QString protocol, QString dtid, QString payload)
{
    QString uptime;
    QString health;
    QString mode;
    QString vendor;
    if (protocol.toInt())
    {
        if (uavcan->getDataType(dtid) == "uavcan.protocol.NodeStatus") // 341 = uavcan.protocol.NodeStatus
        {
            int sec = payload.split(' ').at(0).toInt();
            uptime = QTime::fromMSecsSinceStartOfDay(sec * 1000).toString();
            health = uavcan->getDronecanHealth(payload.split(' ').at(1));
            mode = uavcan->getDronecanMode(payload.split(' ').at(3));
            vendor = "0x" + QString::number(payload.split(' ').at(4).toInt(), 16);
            appendNodeTable(node_id, uptime, health, mode, vendor);
        }
    }
}

void UavcanMonitor::transfetDataUpdate(QString src, QString dst, QString protocol, QString dtid, QString payload)
{
    appendTransferTable(src, dst, protocol, dtid);
    tfPayloadLookupTable[src][uavcan->getDataType(dtid)] = payload;
    qDebug() <<"something";
    updateDetail(currentRow);
    // qDebug() << currentRow;
}

void UavcanMonitor::updateDetail(int row)
{
    if (transferTable->rowCount() == 0)
    {
        return;
    }
    payloadDetails->clear();
    QString key = transferTable->item(row, 0)->text();
    QString index = transferTable->item(row, 3)->text();
    QVector<QString> data = tfPayloadLookupTable[key][index].split(' ');
    QVector<QString> variables =  uavcan->getFields(index).split(", ");
    payloadDetails->addItem(index);
    // qDebug() << "10";
    int dataIndex = 0;

    // combine between variable and value(from payload)
    for (int i = 0; i < variables.length(); i++)
    {
        QVector<QString> variable = variables[i].split(':');
        QString res;
        switch (uavcan->getVariableType(variable.at(1))) {
        case 0: // normal
            res = variable.at(0) + ": ";
            if (dataIndex < data.length())
                res += data[dataIndex++];
            break;
        case 1: // static
            res = variable.at(0) + "[" + variable.at(2) + "]" + ": ";
            for (int s = 0; s < variable.at(2).toInt(); s++)
            {
                if (dataIndex < data.length())
                {
                    res += data[dataIndex++];
                    if (s < variable.at(2).toInt() - 1)
                        res += ", ";
                }
            }
            break;
        case 2: // lessthan
            res = variable.at(0) + "[<" + variable.at(2) + "]" + ": ";
            break;
        case 3: // lessthan equal
            res = variable.at(0) + "[<=" + variable.at(2) + "]" + ": ";
            for (int le = 0; le < variable.at(2).toInt(); le++)
            {
                if (dataIndex < data.length())
                {
                    res += data[dataIndex++];
                    if (le < variable.at(2).toInt() - 1)
                        res += ", ";
                }
            }
            break;
        }
        payloadDetails->addItem(res);
    }
}

void UavcanMonitor::getSelectedRow(int row, int col)
{
    currentRow = row;
    updateDetail(currentRow);
}

