#include "configmanager.h"

ConfigManager::ConfigManager(QObject *parent) :
    QObject(parent)
{
    p_ChatServerConfig = new ChatServerConfig;
    p_LoggerConfig = new LoggerConfig;
}

ConfigManager::~ConfigManager()
{
    delete p_ChatServerConfig;
}

void ConfigManager::ReadConfig()
{
    p_ChatServerConfig->port = m_ServerSettings.value("/tcpserver/port", 33033).toUInt();
    p_LoggerConfig->Path = m_ServerSettings.value("/logger/Path", "Logs").toString();
}

void ConfigManager::sendSignals()
{
    emit configChatServerSignal(p_ChatServerConfig);
    emit configLoggerSignal(p_LoggerConfig);
}

void ConfigManager::WriteConfig()
{
    m_ServerSettings.setValue("/tcpserver/port", p_ChatServerConfig->port);
    m_ServerSettings.setValue("/logger/Path", p_LoggerConfig->Path);
}
