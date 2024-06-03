#ifndef UAVCANDECODER_H
#define UAVCANDECODER_H

#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class UavcanDecoder : public QObject
{
    Q_OBJECT
public:
    explicit UavcanDecoder(QObject *parent = nullptr);

    QString getProtocolType(QString);
    QString getDataType(QString);
    QString getFields(QString);
    int getVariableType(QString);
    QString getDronecanHealth(QString);
    QString getDronecanMode(QString);

signals:

private:
    QJsonDocument jsonDoc;

    // map dtid to datatype
    QMap<QString, QString> datatypeCache;

    // map dtid to field
    QMap<QString, QString> fieldsCache;

    // map dtid -> variable name : array size
    QMap<QString, QString> variableCache;

    QMap<QString, int> variable_type;
    QMap<QString, QMap<QString, QString>> dronecanNodeStatus;
    QMap<QString, QMap<QString,QString>> cyphalNodeStatus;
    // QMap<QString, >
    // QString protocol;
    // QString node_id;
    // QString datatype;
};

#endif // UAVCANDECODER_H
