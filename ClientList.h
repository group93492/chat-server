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
    QString &username();
    void setUsername(QString &name);
    QString &userInfo();
    void setUserInfo(QString &info);
    QTcpSocket *userSocket() const;
    void setUserSocket(QTcpSocket *socket);
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
    QString &channelName();
    void setChannelName(QString &name);
    QString &channelDescription();
    void setChannelDescription(QString &desc);
    ChannelType getChannelType() const;
    void setChannelType(ChannelType type);
    void addClient(ConnectedClient *clnt);
    void deleteClient(ConnectedClient *clnt);
//    QVectorIterator<ConnectedClient *> &getIterator();
};

#endif // CLIENTLIST_H
