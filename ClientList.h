#ifndef CLIENTLIST_H
#define CLIENTLIST_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtNetwork/QTcpSocket>

/*
TODO now:
classes for client, channel and channel list
channel list methods for:
- initialize channel list from database
- getting client list for certain channel
- slots for authorization/deauthorization (auth - add to all channels in which he recorded) (disconnect - remove user from all channels he authorizated)
-
*/
/*
TODO for future:
- inviting in channels
- private channels
*/
class ConnectedClient
{
private:
    QString m_username;
    QString m_userInfo;
    QTcpSocket *m_userSocket;
public:
    ConnectedClient(): m_userSocket(NULL) {}
    QString &username();
    void setUsername(QString name);
    QString &userInfo();
    void setUserInfo(QString info);
    QTcpSocket *userSocket() const;
    void setUserSocket(QTcpSocket *socket);
};

class ChatChannel
{
private:
    QString m_channelName;
    QString m_channelDescriprion;
    QString m_channelTopic;
    QVector<ConnectedClient *> m_usrlist;
public:
    ChatChannel() {}
    QString &channelName();
    void setChannelName(QString name);
    QString &channelDescription();
    void setChannelDescription(QString desc);
    QString &channelTopic();
    void setChannelTopic(QString topic);
    void addClient(ConnectedClient *clnt);
    void deleteClient(ConnectedClient *clnt);
    QStringList getClientUsernameList();
//    QVectorIterator<ConnectedClient *> &getIterator();
};

class ChannelList: public QObject
{
Q_OBJECT
private:
    QVector<ConnectedClient> m_generalClientList;
    QMap<QString, ChatChannel> m_channelList;
public:
    explicit ChannelList(QObject *parent = 0): QObject(parent) {}
    void readChannelListFromDB();
    ChatChannel &getChannel(const QString &channelName);
public slots:
    void authorizeClientInList(const QString &username, QTcpSocket *socket);
    void clientDisconnected(QString &username);
};

#endif // CLIENTLIST_H
