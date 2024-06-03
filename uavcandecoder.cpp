#include "uavcandecoder.h"

UavcanDecoder::UavcanDecoder(QObject *parent) : QObject(parent)
{
    QFile file(":/dronecan.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        // open file error
        qDebug() << "can't open .json file";
        return ;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    jsonDoc = QJsonDocument::fromJson(jsonData);

    if (jsonDoc.isEmpty())
    {
        // read error
        qDebug() << "jsonDoc is emthy";
        return;
    }
    variable_type["n"] = 0;
    variable_type["s"] = 1;
    variable_type["l"] = 2;
    variable_type["le"] = 3;

    dronecanNodeStatus["health"]["0"] = "OK";
    dronecanNodeStatus["health"]["1"] = "WARNING";
    dronecanNodeStatus["health"]["2"] = "ERROR";
    dronecanNodeStatus["health"]["3"] = "CRITICAL";

    dronecanNodeStatus["mode"]["0"] = "OPERATIONAL";
    dronecanNodeStatus["mode"]["1"] = "INITIALIZATION";
    dronecanNodeStatus["mode"]["2"] = "MAINTENANCE";
    dronecanNodeStatus["mode"]["3"] = "SOFTWARE_UPDATE";
    dronecanNodeStatus["mode"]["7"] = "OFFLINE";

}

QString UavcanDecoder::getProtocolType(QString protocol)
{
    switch (protocol.toInt()) {
    case 0:
        return "Cyphal";
    default:
        return "DroneCAN";
    }
}

QString UavcanDecoder::getDataType(QString datatype_id)
{
    if (!datatypeCache[datatype_id].isEmpty())
    {
        return datatypeCache[datatype_id];
    }
    for (QJsonValue obj : jsonDoc.array())
    {
        // qDebug() << obj["default_dtid"].toInt();
        if (obj["default_dtid"].toInt() == datatype_id.toInt())
        {
            datatypeCache[datatype_id] = obj["full_name"].toString();
            return datatypeCache[datatype_id];
        }
    }
    return "";
}

QString UavcanDecoder::getFields(QString datatype)
{
    // format from json file is "[variable name1, variable name2]"
    // we need to tim the message and return
    // "variable name1, variable name2"
    QString field;
    if (!fieldsCache[datatype].isEmpty())
    {
        return variableCache[datatype];
    }
    for (QJsonValue dtid : jsonDoc.array())
    {
        // qDebug() << dtid["full_name"];
        if (dtid["full_name"].toString() == datatype)
        {
            field = dtid["fields"].toString();
            break;
        }
    }
    field = field.mid(1, field.length() - 2).trimmed();
    fieldsCache[datatype] = field;
    // we got "variable name1, variable name2"
    // then we need name1:array_size, name2:array_size
    QVector<QString> variable_name = field.split(", ");
    // we get name[array_size]
    // but array have dynamic array size : < , <=
    // we will classify each type static_size = name:s:n, lessthan = name:l:n, lessthan_equal = name:le:n
    // then this process will split name[array_size] and combine -> name:type:size, ....

    // trimming
    QString variableList;
    for (int i = 0; i < variable_name.length(); i++)
    {
        QString res;
        if (variable_name[i].contains("["))
        {
            QString name = variable_name[i].split(' ').at(1); // name
            QString array_size = variable_name[i].mid(variable_name[i].indexOf('['));
            array_size = array_size.mid(1, array_size.indexOf(']') - 1).trimmed();
            QString type = "s"; // type default
            QString size = array_size; // size default
            if (array_size.contains("<", Qt::CaseSensitive))
            {
                type = "l";
                size = array_size.mid(1);
            }
            if (array_size.contains("=", Qt::CaseSensitive))
            {
                type = "le";
                size = array_size.mid(2);
            }
            res = name + ":" + type + ":" + size;
        }
        else
        {
            // QString name = variable_name[i].split('[').at(0); // name
            // name:type normal:size=0
            QString name;
            if (variable_name[i].mid(0, 4) == "void")
            {
                name = "void";
            }
            else
                name = variable_name[i].split(' ').at(1); // name
            res = name + ":" + "n" + ":" + "0";
        }
        variableList.append(res);
        if (i < variable_name.length() - 1)
            variableList.append(", ");
    }
    variableCache[datatype] = variableList;
    return variableCache[datatype];
}

int UavcanDecoder::getVariableType(QString type)
{
    return variable_type[type];
}

QString UavcanDecoder::getDronecanHealth(QString status)
{
    return dronecanNodeStatus["health"][status];
}

QString UavcanDecoder::getDronecanMode(QString status)
{
    return dronecanNodeStatus["mode"][status];
}


