#include "configmanager.h"

ConfigManager::ConfigManager(QObject *parent) :
    QObject(parent)
{
    ptr = new ChatServerConfig;
}

void ConfigManager::ReadConfig()
{
    ptr->port = ServerSettings.value("port", 33033).toUInt();
}

void ConfigManager::WriteConfig(QString key, QString value)
{
    ServerSettings.setValue(key, value);
}
