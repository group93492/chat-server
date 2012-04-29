#include "ClientList.h"

QString &ChatClient::username()
{
    return m_username;
}

void ChatClient::setUsername(QString name)
{
    m_username = name;
}

QString &ChatClient::userInfo()
{
    return m_userInfo;
}

void ChatClient::setUserInfo(QString info)
{
    m_userInfo = info;
}

QString &ChatClient::password()
{
    return m_userPassword;
}

void ChatClient::setPassword(QString pass)
{
    m_userPassword = pass;
}

QTcpSocket *ChatClient::userSocket() const
{
    return m_userSocket;
}

void ChatClient::setUserSocket(QTcpSocket *socket)
{
    m_userSocket = socket;
}

QString &ChatChannel::name()
{
    return m_name;
}

void ChatChannel::setName(QString name)
{
    m_name = name;
}

QString &ChatChannel::description()
{
    return m_description;
}

void ChatChannel::setDescription(QString desc)
{
    m_description = desc;
}

void ChatChannel::addClient(QString username)
{
    userList.append(username);
}

void ChatChannel::deleteClient(QString username)
{
    userList.removeAll(username);
}

bool ChatChannel::hasClient(QString username)
{
    return userList.contains(username);
}

QString &ChatChannel::topic()
{
    return m_topic;
}

void ChatChannel::setTopic(QString topic)
{
    m_topic = topic;
}

DBManager::DBManager(QObject *parent) :
    QObject(parent),
    m_clientTableName("clients"),
    m_channelTableName("channels"),
    m_membershipTableName("membership"),
    m_DBFileName("chat.db")
{
    connectToDB();
}

DBManager::~DBManager()
{
    disconnectDB();
}

void DBManager::connectToDB()
{
    QString msg;
    QFile file;
    file.setFileName(m_DBFileName);
    m_DB = QSqlDatabase::addDatabase("QSQLITE");
    if(file.exists())
    {
        m_DB.setDatabaseName(m_DBFileName);
        if(!m_DB.open())
        {
            msg = m_DB.lastError().text();
            emit logMessage(esFatal, msg);
        }
        else
        {
            msg = "DataBase opened.";
            emit logMessage(esNotify, msg);
        }

    }
    else
    {
        m_DB.setDatabaseName(m_DBFileName);
        if(!m_DB.open())
        {
            msg = m_DB.lastError().text();
            emit logMessage(esFatal, msg);
        }
        else
        {
            msg = "DataBase not found, creating new DataBase";
            emit logMessage(esNotify, msg);
        }
        createDB();
    }
}

void DBManager::disconnectDB()
{
    m_DB.close();
}

bool DBManager::createClientsTable()
{
    QSqlQuery query;
    QString str = "CREATE TABLE " + m_clientTableName + " ( "
            "name           VARCHAR(25) PRIMARY KEY ASC, "
            "password       VARCHAR(20), "
            "info           VARCHAR(200) "
            ");";
    return query.exec(str);
}

bool DBManager::createChannelsTable()
{
    QString msg;
    QSqlQuery query;
    QString str = "CREATE TABLE " + m_channelTableName+ " ( "
            "name           VARCHAR(25) PRIMARY KEY ASC, "
            "topic          VARCHAR(200), "
            "description    VARCHAR(200)"
            ");";
    return query.exec(str);
}

bool DBManager::createMembershipTable()
{
    QSqlQuery query;
    QString str = "CREATE TABLE " + m_membershipTableName + " ( "
            "channel       VARCHAR(25), "
            "client          VARCHAR(25) "
            ");";
    return query.exec(str);
}

void DBManager::createDB()
{

    QString msg;
    if (!createClientsTable() ||
        !createChannelsTable() ||
        !createMembershipTable())
    {
        msg = "Error creating tables";
        emit logMessage(esFatal, msg);
    }
    else
    {
        msg = "Tables 'clients', 'channels', 'membership' created succesfully";
        emit logMessage(esNotify, msg);
    }
    ChatChannel channel;
    channel.setName("main");
    channel.setDescription("Main channel, contains all users");
    channel.setTopic("Gimme fu, gimme fai, gimme dabajabazza");
    setChannel(channel);
}

bool DBManager::hasClient(QString username)
{
    QSqlQuery query;
    QString str = QString("SELECT * FROM '%1' WHERE name = '%2';")
                    .arg(m_clientTableName)
                    .arg(username);
    if (!query.exec(str))
    {
        QString msg("SQL query error in getting client: %1");
        msg.arg(query.lastError().text());
        emit logMessage(esWarning, msg);
    }
    else
        return query.next();
    return false;
}

ChatClient DBManager::getClient(QString username)
{
    ChatClient client;
    QSqlQuery query;
    QString str = QString("SELECT name, password, info FROM '%1' WHERE name = '%2';")
                    .arg(m_clientTableName)
                    .arg(username);
    if (!query.exec(str) || !query.next())
    {   
        QString msg("SQL query error in getting client: %1");
        msg.arg(query.lastError().text());
        emit logMessage(esWarning, msg);
    }
    else
    {
        // create new client and fill it with results of query
        // maybe we should use smart pointer here
        client.setUsername(query.value(0).toString());
        client.setPassword(query.value(1).toString());
        client.setUserInfo(query.value(2).toString());
    }
    return client;
}

//"REPLACE INTO dummies (rowid, name, info)"
//"VALUES (null, '%1', '%2');"
void DBManager::setClient(ChatClient &client)
{
    QSqlQuery query;
    QString q = QString("REPLACE INTO %1 (rowid, name, password, info) "
                        "VALUES (null, '%2', '%3', '%4')")
                        .arg(m_clientTableName)
                        .arg(client.username())
                        .arg(client.password())
                        .arg(client.userInfo());
    if (!query.exec(q))
    {
        QString msg("SQL query error in setting client: %1");
        msg.arg(query.lastError().text());
        emit logMessage(esWarning, msg);
    }
}

bool DBManager::hasChannel(QString channelName)
{
    QSqlQuery query;
    QString str = QString("SELECT * FROM '%1' WHERE name = '%2';")
                    .arg(m_channelTableName)
                    .arg(channelName);
    if (!query.exec(str))
    {
        QString msg("SQL query error in getting channel: %1");
        msg.arg(query.lastError().text());
        emit logMessage(esWarning, msg);
        return false;
    }
    else
        return query.next();
}

ChatChannel DBManager::getChannel(QString channelName)
{
    ChatChannel channel;
    QSqlQuery query;
    QString str = QString("SELECT name, topic, description FROM '%1' WHERE name = '%2';")
                    .arg(m_channelTableName)
                    .arg(channelName);
    if (!query.exec(str) || !query.next())
    {
        QString msg("SQL query error in getting channel: %1");
        msg.arg(query.lastError().text());
        emit logMessage(esWarning, msg);
    }
    else
    {
        // create new client and fill it with results of query
        // maybe we should use smart pointer here
        channel.setName(query.value(0).toString());
        channel.setTopic(query.value(1).toString());
        channel.setDescription(query.value(2).toString());
    }
    return channel;
}

void DBManager::setChannel(ChatChannel &channel)
{
    QSqlQuery query;
    QString q = QString("REPLACE INTO %1 (rowid, name, topic, description) "
                        "VALUES (null, '%2', '%3', '%4')")
                        .arg(m_channelTableName)
                        .arg(channel.name())
                        .arg(channel.topic())
                        .arg(channel.description());
    if (!query.exec(q))
    {
        QString msg("SQL query error in setting channel: %1");
        msg.arg(query.lastError().text());
        emit logMessage(esWarning, msg);
    }
}

bool DBManager::isMembership(QString username, QString channelName)
{
    QSqlQuery query;
    QString q = QString("SELECT * FROM %1 WHERE channel = '%2' AND client = '%3'")
                        .arg(m_membershipTableName)
                        .arg(channelName)
                        .arg(username);
    if (!query.exec(q))
    {
        QString msg("SQL query error in getting membership: %1");
        msg.arg(query.lastError().text());
        emit logMessage(esWarning, msg);
        return false;
    }
    else
        return query.next();
}

void DBManager::addMembership(QString username, QString channelName)
{
    QSqlQuery query;
    QString q = QString("INSERT INTO %1 (rowid, client, channel) "
                        "VALUES (null, '%2', '%3')")
                        .arg(m_membershipTableName)
                        .arg(username)
                        .arg(channelName);
    if (!query.exec(q))
    {
        QString msg("SQL query error in adding membership: %1");
        msg.arg(query.lastError().text());
        emit logMessage(esWarning, msg);
    }
}

void DBManager::deleteMembership(QString username, QString channel)
{
    QSqlQuery query;
    QString q = QString("DELETE FROM '%1' WHERE client = '%3' AND channel = '%4';")
                                                    .arg(m_membershipTableName)
                                                    .arg(username)
                                                    .arg(channel);
    if (!query.exec(q))
    {
        QString msg("SQL query error in deleting membership: %1");
        msg.arg(query.lastError().text());
        emit logMessage(esWarning, msg);
    }
}

QVector<ChatChannel> DBManager::getChannelList()
{
    QVector<ChatChannel> channels;
    ChatChannel tempChannel;
    QSqlQuery query;
    QString q = QString("SELECT name, description, topic FROM %1")
                        .arg(m_channelTableName);
    if (!query.exec(q) || !(query.next()))
    {
        QString msg("SQL query error in getting channel list: %1");
        msg.arg(query.lastError().text());
        emit logMessage(esWarning, msg);
//        return channels;
    }
    do
    {
        tempChannel.setName(query.value(0).toString());
        tempChannel.setDescription(query.value(1).toString());
        tempChannel.setTopic(query.value(2).toString());
        channels.append(tempChannel);
    } while (query.next());
    return channels;
}

ChatChannel GeneralClientList::getChannel(QString &channelName)
{
    for (int i = 0; i < m_channelList.count(); ++i)
        if (m_channelList[i].name() == channelName)
            return m_channelList[i];
}

bool GeneralClientList::hasChannel(QString channelName)
{
    bool found = false;
    for (int i = 0; i < m_channelList.count(); ++i)
        if (m_channelList[i].name() == channelName)
        {
            found = true;
            break;
        }
    return found;
}

ChatClient GeneralClientList::getClient(const QString &username)
{
    return m_generalClientList.value(username);
}

bool GeneralClientList::hasClient(QString username)
{
    return m_generalClientList.contains(username);
}

QMap<QString, QString> GeneralClientList::getChannelsForClient(QString username)
{
    QMap<QString, QString> channels;
    for (int i = 0; i < m_channelList.count(); ++i)
        if (m_DB.isMembership(username, m_channelList[i].name()))
            channels.insert(m_channelList[i].name(), m_channelList[i].description());
    return channels;
}

QMap<QString, QString> GeneralClientList::getAllChanells()
{
    QMap<QString, QString> channels;
    for (int i = 0; i < m_channelList.count(); ++i)
        channels.insert(m_channelList[i].name(), m_channelList[i].description());
    return channels;
}

GeneralClientList::RegResult GeneralClientList::registrate(QString username, QString password)
{
    if (m_DB.hasClient(username))
        return GeneralClientList::rrOccupiedUsername;
    //check username for not conflicting with our architecture
    //if conflicts - return GeneralClientList::rrBadUsername
    ChatClient newClient;
    newClient.setUsername(username);
    newClient.setPassword(password);
    newClient.setUserInfo(username + " - MALACA");  //default userinfo
    m_DB.setClient(newClient);
//    m_DB.addMembership(username, "main");
    //autojoining all channels - shold be deleted soon
    for (int i = 0; i < m_channelList.count(); ++i)
        m_DB.addMembership(username, m_channelList[i].name());
    //
    return GeneralClientList::rrRegSuccess;
}

GeneralClientList::GeneralClientList(QObject *parent): QObject(parent)
{
    //readChannelsFromDB();
    connect(&m_DB, SIGNAL(logMessage(ErrorStatus, QString &)), this, SLOT(replyLog(ErrorStatus, QString&)));
}

GeneralClientList::AuthResult GeneralClientList::authorize(QString username, QString password, QTcpSocket *socket)
{
    //comparing authorization data
    //if the client doesn't exists in our DB - abort
    if (!m_DB.hasClient(username))
        return GeneralClientList::arWrongAuthData;
    //if password is incorrect - abort
    ChatClient authClient = m_DB.getClient(username);
    if (password != authClient.password())
        return GeneralClientList::arWrongAuthData;
    //if client is allready authorized - abort
    if (this->hasClient(username))
        return GeneralClientList::arAllreadyAuthorized;
    //ok, authorization tests passed
    //add client to general list
    authClient.setUserSocket(socket);
    m_generalClientList.insert(username, authClient);
    //and add him to channel lists
    for (int i = 0; i < m_channelList.count(); ++i)
        if (m_DB.isMembership(username, m_channelList[i].name()))
            m_channelList[i].addClient(username);
    //ok, thats all
    return GeneralClientList::arAuthSuccess;
}

void GeneralClientList::disconnect(QString username)
{
    if (!this->hasClient(username))
        return;
    m_generalClientList.value(username).userSocket()->close();
    //socket->close();
    //delete client from channels
    for (int i = 0; i < m_channelList.count(); ++i)
        if (m_channelList[i].hasClient(username))
            m_channelList[i].deleteClient(username);
    //delete client from general list
    m_generalClientList.remove(username);
}

void GeneralClientList::joinChannel(QString username, QString channelName)
{
    /*if (!hasClient(username)) //joining channel supported only for connected clients
        return;*/
    if (m_DB.isMembership(username, channelName)) // we dont need to join channel if we are allready in it
        return;
    m_DB.addMembership(username, channelName);
    for (int i = 0; i < m_channelList.count(); ++i)
        if (channelName == m_channelList[i].name())
        {
            m_channelList[i].addClient(username);
            break;
        }
    //we need to refresh list of channel membership on client-part
}

void GeneralClientList::leaveChannel(QString username, QString channelName)
{
    if (channelName == "main" || !m_DB.isMembership(username, channelName))
        return;
    m_DB.deleteMembership(username, channelName);
}

GeneralClientList::CreateChannelResult GeneralClientList::createChannel(QString username, QString channelName, QString description, QString topic)
{
    int maxChannelCount = 50;
    if (m_channelList.count() >= maxChannelCount)
        return ccrTooManyChannels;
    for (int i = 0; i < m_channelList.count(); i++)
        if (m_channelList[i].name() == channelName)
            return ccrBadName;
    ChatChannel newChannel;
    newChannel.setDescription(description);
    newChannel.setName(channelName);
    newChannel.setTopic(topic);
    m_DB.setChannel(newChannel);
    m_channelList.append(newChannel);
    return ccrSuccess;
}

void GeneralClientList::replyLog(ErrorStatus status, QString &param)
{
    emit logMessage(status, param);
}

void GeneralClientList::readChannelsFromDB()
{
    m_channelList = m_DB.getChannelList();
}
