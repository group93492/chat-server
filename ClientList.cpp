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

void ChatChannel::setChannelDescription(QString desc)
{
    m_description = desc;
}

void ChatChannel::addClient(ChatClient &clnt)
{
    userList.push_back(clnt);
}

void ChatChannel::deleteClient(ChatClient &clnt)
{
    quint16 index = m_usrlist.indexOf(clnt);
    userList.remove(index);
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
    disconnectBase();
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
            msg = m_DB.lastError().text();
        else
            msg = "DataBase opened.";
        emit logMessage(msg);
    }
    else
    {
        m_DB.setDatabaseName(m_DBFileName);
        if(!m_DB.open())
            msg = m_DB.lastError().text();
        else
            msg = "DataBase not found, creating new DataBase";
        emit logMessage(msg);
        createDB();
    }
}

void DBManager::disconnectDB()
{
    if (tableView)
        tableView->close();
    DataBase.close();
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

void DBManager::lookTable(QTableView *widget, QString tablename)
{
    tableView = widget;
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable(tablename);
    model->select();
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    tableView->setModel(model);
}


bool DBManager::authorization(QString nick, QString password)
{
    QString msg;
    QSqlQuery query;
    QString str = QString("SELECT nick, password FROM %1 WHERE nick = %2;")
            .arg(TableClientsName)
            .arg("'" + nick + "'");
    if(!query.exec(str))
    {
        msg = query.lastError().text();
        emit logMessage(msg);
        return false;
    }
    else
        if(query.next() && query.value(0).toString() == nick && query.value(1).toString() == password)
            return true;
        else
            return false;
}

void DBManager::createDB()
{
    if (!createClientsTable() ||
        !createChannelsTable() ||
        !createMembershipTable())
        emit logMessage("Error creating tables");
    ChatChannel channel;
    channel.setName("Main");
    channel.setDescription("Main channel, contains all users");
    channel.setTopic("Gimme fu, gimme fai, gimme dabajabazza");
    setChannel(channel);
}

ChatClient *DBManager::getClient(QString username)
{
    ChatClient *client = NULL;
    QSqlQuery query;
    QString str = QString("SELECT name, password, info FROM '%1'' WHERE name = '%2';")
                    .arg(m_clientTableName)
                    .arg(username);
    if (!query.exec(str) || !query.next())
    {
        emit logMessage(QString("SQL query error in getting client: %1").arg(query.lastError().text()));
    }
    else
    {
        // create new client and fill it with results of query
        // maybe we should use smart pointer here
        client = new ChatClient;
        client->setUsername(query.value(0).toString());
        client->setPassword(query.value(1).toString());
        client->setUserInfo(query.value(2).toString());
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
        emit logMessage(QString("SQL query error in setting client: %1").arg(query.lastError().text()));
}

ChatChannel *DBManager::getChannel(QString channelName)
{
    ChatChannel *channel= NULL;
    QSqlQuery query;
    QString str = QString("SELECT name, topic, description FROM '%1' WHERE name = '%2';")
                    .arg(m_channelTableName)
                    .arg(channelName);
    if (!query.exec(str) || !query.next())
    {
        emit logMessage(QString("SQL query error in getting channel: %1").arg(query.lastError().text()));
    }
    else
    {
        // create new client and fill it with results of query
        // maybe we should use smart pointer here
        channel = new ChatChannel;
        channel->setName(query.value(0).toString());
        channel->setTopic(query.value(1).toString());
        channel->setDescription(query.value(2).toString());
    }
    return client;
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
        emit logMessage(QString("SQL query error setting channel: %1").arg(query.lastError().text()));
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
        emit logMessage(QString("SQL query error getting membership: %1").arg(query.lastError().text()));
        return false;
    }
    else
        return query.next();
}

void DBManager::addMembership(QString username, QString channelName)
{
    QSqlQuery query;
    QString q = QString("INSERT INTO %1 (rowid, channel, client) "
                        "VALUES (null, '%2', '%3')")
                        .arg(m_membershipTableName)
                        .arg(username)
                        .arg(channelName);
    if (!query.exec(q))
        emit logMessage(QString("SQL query error adding membership: %1").arg(query.lastError().text()));
}

void GeneralClientList::readChannelListFromDB()
{
    //open db and read all the channels into m_channelList where key=channelName

    //now there's dummy for creating new main channel
    ChatChannel newChannel;
    newChannel.setChannelName(QString("main"));
    newChannel.setChannelDescription("Kokoko");
    newChannel.setChannelTopic("GLAVNAYA PETUSHATNYA TUT");
    m_channelList.insert("main", newChannel);
}

ChatChannel &GeneralClientList::getChannel(const QString &channelName)
{
    return m_channelList[channelName];
}

ChatClient &GeneralClientList::getClient(const QString &username)
{
    return m_generalClientList.value(username);
}

bool GeneralClientList::hasClient(QString username)
{
    return m_generalClientList.contains(username);
}

void GeneralClientList::addClient(const QString &username, QTcpSocket *socket)
{
    ChatClient newClient;
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

void GeneralClientList::removeClient(QString &username)
{
    //remove client from list
}

QStringList &GeneralClientList::getChannelsForClient(QString username)
{
    QStringList channels;
    QMap<QString, ChatChannel>::iterator channelIt = m_channelList.begin();
    for (; channelIt != m_channelList.end(); channelIt++)
        if (channelIt.value().hasClient(username))
            channels.append(channelIt.key());
    return channels;
}

ChatChannel &GeneralClientList::getChannel(QString channelName)
{
    return m_channelList.value(channelName);
}

GeneralClientList::GeneralClientList(QObject *parent): QObject(parent)
{
    readChannelsFromDB();
}

GeneralClientList::AuthResult GeneralClientList::authorize(QString username, QString password)
{
    //comparing authorization data
    if (this->hasClient(usename))
        return GeneralClientList::arAllreadyAuthorized;
    ChatClient newClient = m_DB.getClient(username);
    if (!newClient)
        return GeneralClientList::arWrongAuthData;
    if (password != newClient.password())
        return GeneralClientList::arWrongAuthData;
    //ok, authorization passed
    //add client to general list
    m_generalClientList.insert(newClient.username(), newClient);
    //and add him to channel lists
    QMap<QString, ChatChannel>::iterator channelIt = m_channelList.begin();
    for (; channelIt != m_channelList.end(); channelIt++)
        if (m_DB.isMembership(newClient.username(), channelIt.value().name()))
            channelIt.value().addClient(newClient);
    //ok, thats all
    return GeneralClientList::arAuthSuccess;
}

void GeneralClientList::disconnect(QString username)
{
    if (!this->hasClient(username))
        return;
    m_generalClientList.value(username).userSocket()->close;
    //socket->close();
    //delete client from channels
    QMap<QString, ChatChannel>::iterator channelIt = m_channelList.begin();
    for (; channelIt != m_channelList.end(); channelIt++)
        if (channelIt.value().hasClient(username))
            channelIt.value().deleteClient(username);
    //delete client from general list
    m_generalClientList.remove(username);
}

void GeneralClientList::joinChannel(QString username, QString channelName)
{
    if (!hasClient(username)) //joining channel supported only for connected clients
        return;
    if (m_DB.isMembership(username, channelName)) // we dont need to join channel if we are allready in it
        return;
    m_DB.addMembership(username, channelName);
    m_channelList.value(channelName).addClient(m_generalClientList.value(username));
    //we need to refresh list of channel membership on client-part
}

void GeneralClientList::leaveChannel(QString username, QString channelName)
{
}

void GeneralClientList::readChannelsFromDB()
{
}
