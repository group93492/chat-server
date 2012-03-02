#include "ClientList.h"

QString &ConnectedClient::username()
{
    return m_username;
}

void ConnectedClient::setUsername(QString name)
{
    m_username = name;
}

QString &ConnectedClient::userInfo()
{
    return m_userInfo;
}

void ConnectedClient::setUserInfo(QString info)
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

void ChatChannel::setChannelName(QString name)
{
    m_channelName = name;
}

QString &ChatChannel::channelDescription()
{
    return m_channelDescriprion;
}

void ChatChannel::setChannelDescription(QString desc)
{
    m_channelDescriprion = desc;
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

QStringList ChatChannel::getClientUsernameList()
{
    QStringList usernames;
    for (int i = 0; i < m_usrlist.count(); i++)
        usernames.append(m_usrlist[i]->username());
    return usernames;
}

QString &ChatChannel::channelTopic()
{
    return m_channelTopic;
}

void ChatChannel::setChannelTopic(QString topic)
{
    m_channelTopic = topic;
}

void ChannelList::readChannelListFromDB()
{
    //open db and read all the channels into m_channelList where key=channelName

    //now there's dummy for creating new main channel
    ChatChannel newChannel;
    newChannel.setChannelName(QString("main"));
    newChannel.setChannelDescription("Kokoko");
    newChannel.setChannelTopic("GLAVNAYA PETUSHATNYA TUT");
    m_channelList.insert("main", newChannel);
}

ChatChannel &ChannelList::getChannel(const QString &channelName)
{
    return m_channelList[channelName];
}

void ChannelList::authorizeClientInList(const QString &username, QTcpSocket *socket)
{
    ConnectedClient newClient;
    //get information about client from db
    newClient.setUsername(username);
    newClient.setUserSocket(socket);
    newClient.setUserInfo(username + " - malaca");
    //add new client to m_generalClientList
    m_generalClientList.push_back(newClient);
    //get all channels in those client recorded from db
    //nowhere to get
    //and add pointer to newClient to every channel in which he is
    m_channelList["main"].addClient(&newClient);
}
