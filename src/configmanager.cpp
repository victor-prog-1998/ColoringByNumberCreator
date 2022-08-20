#include "include/configmanager.h"
#include <QFile>
#include <QJsonDocument>

ConfigManager::ConfigManager(QObject *parent) : QObject(parent)
{
    _init();
}

void ConfigManager::setConfigs(bool simplify, const QString &coloringColor,
                               int scalingFactor)
{
    m_configsJson["Simplify"] = simplify;
    m_configsJson["ColoringColor"] = coloringColor;
    m_configsJson["ScalingFactor"] = scalingFactor;
    _writeConfigsToFile();
}

void ConfigManager::_init()
{
    if(!QFile::exists(m_fileName))
        setConfigs(m_simplify, m_coloringColor);
    else
        _readConfigsFromFile();
}

void ConfigManager::_writeConfigsToFile()
{
    QFile configsFile(m_fileName);
    configsFile.open(QIODevice::WriteOnly);
    configsFile.write(QJsonDocument(m_configsJson).toJson());
    configsFile.close();
}

void ConfigManager::_readConfigsFromFile()
{
    QFile configsFile(m_fileName);
    configsFile.open(QIODevice::ReadOnly);
    QByteArray bytes = configsFile.readAll();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(bytes);
    m_configsJson = jsonDocument.object();
}
