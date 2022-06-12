#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QJsonObject>

class ConfigManager : public QObject
{
    Q_OBJECT
public:
    explicit ConfigManager(QObject *parent = nullptr);
    Q_INVOKABLE void setConfigs(bool simplify, const QString& coloringColor);
    Q_INVOKABLE bool simplify() const {return m_configsJson["Simplify"].toBool();}
    Q_INVOKABLE QString coloringColor() const {return  m_configsJson["ColoringColor"].toString();}

private:
    void _init();
    void _writeConfigsToFile();
    void _readConfigsFromFile();
    const QString m_fileName {"Configs.json"};
    bool m_simplify {false};
    QString m_coloringColor {"black"};
    QJsonObject m_configsJson;

signals:

};

#endif // CONFIGMANAGER_H
