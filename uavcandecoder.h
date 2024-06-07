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

    // map dtid to data type
    QMap<QString, QString> datatypeCache;

    // map dtid to field
    QMap<QString, QString> fieldsCache;

    // map dtid -> variable name : array size
    QMap<QString, QString> variableCache;

    QMap<QString, int> variable_type;
};

#endif // UAVCANDECODER_H
