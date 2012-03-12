#include "configmanager.h"

ConfigManager::ConfigManager(QObject *parent) :
    QObject(parent)
{
    p_ChatServerConfig = new ChatServerConfig;
}

ConfigManager::~ConfigManager()
{
    delete p_ChatServerConfig;
}

void ConfigManager::ReadConfig()
{
    p_ChatServerConfig->port = ServerSettings.value("/tcpserver/port", 33033).toUInt();
}

void ConfigManager::sendSignals()
{
    emit ChatServerSignal(p_ChatServerConfig);
}

void ConfigManager::WriteConfig()
{
    ServerSettings.setValue("/tcpserver/port", p_ChatServerConfig->port);
}
