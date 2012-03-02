#include "ClientList.h"

QString &ConnectedClient::username()
{
    return m_username;
}

void ConnectedClient::setUsername(QString &name)
{
    m_username = name;
}

QString &ConnectedClient::userInfo()
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

QString &ChatChannel::channelName()
{
    return m_channelName;
}

void ChatChannel::setChannelName(QString &name)
{
    m_channelName = name;
}

QString &ChatChannel::channelDescription()
{
    return m_channelDescriprion;
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

ChannelType ChatChannel::getChannelType() const
{
    return m_channelType;
}

void ChatChannel::setChannelType(ChannelType type)
{
    m_channelType = type;
}
