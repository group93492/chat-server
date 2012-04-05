#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QSettings>

struct ChatServerConfig
{
    quint16 port;
};

struct LoggerConfig
{
    QString Path;
};

class ConfigManager : public QObject
{
    Q_OBJECT
private:
    QSettings m_ServerSettings;
public:
    explicit ConfigManager(QObject *parent = 0);
    ~ConfigManager();
    ChatServerConfig *p_ChatServerConfig;
    LoggerConfig *p_LoggerConfig;
    void ReadConfig();
    void sendSignals();
signals:
    void configChatServerSignal(ChatServerConfig *p_ChatServerConfig);
    void configLoggerSignal(LoggerConfig *p_LoggerConfig);
public slots:
    void WriteConfig();
};

#endif // CONFIGMANAGER_H
