#include "ClientList.h"

QString &ConnectedClient::username() const
{
    return m_username;
}

void ConnectedClient::setUsername(QString &name)
{
    m_username = name;
}

QString &ConnectedClient::userInfo() const
{
    return m_userInfo;
}

void ConnectedClient::setUserInfo(QString &info)
{
    m_userInfo = info;
}

QTcpSocket *ConnectedClient::userSocket() const
{
    return m_userSocket;
}

void ConnectedClient::setUserSocket(QTcpSocket *socket)
{
    m_userSocket = socket;
}

void ClientList::clientAuthorized(QString &username) const
{
    return m_usrlist.contains(username);
}

void ClientList::addClient(QString &username, QTcpSocket *socket)
{
    if (clientAuthorized(username))
        return;
    ConnectedClient client;
    client.setUserSocket(socket);
    //pull information about <%username> from database and fill <client> with this information
    client.setUsername(username);
    client.setUserInfo(QString("%1 - malaca").arg(username));
    m_usrlist.insert(username, client);
}

void ClientList::deleteClient(QString &username)
{
    if (!clientAuthorized(username))
        return;
    m_usrlist.remove(username);
}

ConnectedClient &ClientList::getClient(QString &username) const
{
    return m_usrlist.value(username, NULL);
}

QString &ChatChannel::channelName() const
{
    return m_channelName;
}

void ChatChannel::setChannelName(QString &name)
{
    m_channelName = name;
}

QString &ChatChannel::channelDescription() const
{
    return m_channelDescriprion;
}

QVectorIterator<ConnectedClient *> &ChatChannel::getIterator()
{
    return QVectorIterator(m_usrlist);
}

void ChatChannel::addClient(ConnectedClient *clnt)
{
    m_usrlist.push_back(clnt);
}

void ChatChannel::deleteClient(ConnectedClient *clnt)
{
    quint16 index = m_usrlist.indexOf(clnt);
    m_usrlist.remove(index);
}

ChatChannel::ChannelType ChatChannel::getChannelType() const
{
    return m_channelType;
}

void ChatChannel::setChannelType(ChatChannel::ChannelType type)
{
    m_channelType = type;
}
