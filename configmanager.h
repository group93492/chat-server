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
    ChatServerConfig *ptr;
public:
    explicit ConfigManager(QObject *parent = 0);
    void ReadConfig();
signals:
    void ChatServerSignal(ChatServerConfig *ptr);
public slots:
    void WriteConfig(QString key, QString value);
};

#endif // CONFIGMANAGER_H
