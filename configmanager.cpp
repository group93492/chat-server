#include "configmanager.h"

ConfigManager::ConfigManager(QObject *parent) :
    QObject(parent)
{
    p_ChatServerConfig = new ChatServerConfig;
}

void ConfigManager::ReadConfig()
{
    p_ChatServerConfig->port = ServerSettings.value("/tcpserver/port", 33033).toUInt();
    emit ChatServerSignal(p_ChatServerConfig);
}

void ConfigManager::WriteConfig(QString key, QString value)
{
    ServerSettings.setValue(key, value);
}
