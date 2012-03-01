#ifndef CLIENTLIST_H
#define CLIENTLIST_H

#include <QString>
#include <QtNetwork/QTcpSocket>

class ConnectedClient
{
private:
    QString m_username;
    QString m_userInfo;
    QTcpSocket *m_userSocket;
public:
    ConnectedClient(): m_userSocket(NULL) {}
    QString &username() const;
    void setUsername(QString &name);
    QString &userInfo() const;
    void setUserInfo(QString &info);
    QTcpSocket *userSocket() const;
    void setUserSocket(QTcpSocket *socket);
};

class ClientList
{
private:
     QMap<QString, ConnectedClient> m_usrlist;
public:
     ClientList() {}
     void clientAuthorized(QString &username) const;
     void addClient(QString &username, QTcpSocket *socket);
     void deleteClient(QString &username);
     ConnectedClient &getClient(QString &username) const;
};

enum ChannelType
{
    ctPrivate,
    ctPublic
};

class ChatChannel
{
private:
    QString m_channelName;
    QString m_channelDescriprion;
    QVector<ConnectedClient *> m_usrlist;
    ChannelType m_channelType;
public:
    ChatChannel() {}
    QString &channelName() const;
    void setChannelName(QString &name);
    QString &channelDescription() const;
    void setChannelDescription(QString &desc);
    ChannelType getChannelType() const;
    void setChannelType(ChannelType type);
    void addClient(ConnectedClient *clnt);
    void deleteClient(ConnectedClient *clnt);
    QVectorIterator<ConnectedClient *> &getIterator();
};

class ChannelList
{
private:
    QMap<QString, ChatChannel> m_chanlist;
public:
    ChannelList() {}
    //read channel list from db
    //method for adding private channel

};

#endif // CLIENTLIST_H
