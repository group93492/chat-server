#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QSettings>

struct ChatServerConfig
{
    quint16 port;
};

class ConfigManager : public QObject
{
    Q_OBJECT
private:
    QSettings ServerSettings;
public:
    explicit ConfigManager(QObject *parent = 0);
    ChatServerConfig *p_ChatServerConfig;
    void ReadConfig();
signals:
    void ChatServerSignal(ChatServerConfig *p_ChatServerConfig);
public slots:
    void WriteConfig(QString key, QString value);
};

#endif // CONFIGMANAGER_H
