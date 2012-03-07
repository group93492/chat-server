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

void ChatChannel::addClient(ChatClient *clnt)
{
    m_usrlist.push_back(clnt);
}

void ChatChannel::deleteClient(ChatClient *clnt)
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

QString &ChatChannel::topic()
{
    return m_topic;
}

void ChatChannel::setTopic(QString topic)
{
    m_topic = topic;
}


DBManager::DBManager(QObject *parent) :
    QObject(parent)
{

}

DBManager::DBManager(QString name)
{
    DataBaseName = name;
    TableClientsName = "clients";
    TableChannelsName = "channels";
}

DBManager::~DBManager()
{
    disconnectBase();
}

void DBManager::connectToBase()
{
    QString msg;
    QFile file;
    file.setFileName(DataBaseName);
    QSqlDatabase DataBase = QSqlDatabase::addDatabase("QSQLITE");
    if(file.exists())
    {
        DataBase.setDatabaseName(DataBaseName);
        if(!DataBase.open())
            msg = DataBase.lastError().text();
        else
            msg = "DataBase found";
        emit logMessage(msg);
    }
    else
    {
        DataBase.setDatabaseName(DataBaseName);
        if(!DataBase.open())
            msg = DataBase.lastError().text();
        else
            msg = "DataBase not found, create new DataBase";
        emit logMessage(msg);
        createClientsTable();
        createChannelsTable();
    }
}

void DBManager::disconnectBase()
{
    tableView->close();
    DataBase.close();
}

bool DBManager::createClientsTable()
{
    QSqlQuery query;
    QString str = "CREATE TABLE " + m_clientTableName + " ( "
            "name           VARCHAR(25), "
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
            "name           VARCHAR(25), "
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
            "user          VARCHAR(25) "
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

bool DBManager::addNewClient(QString nick, QString password, QString inf)
{
    QString msg;
    QSqlQuery query;
    QString str = QString("INSERT INTO %1 (nick, password, inf)"
            "VALUES ('%2', '%3', '%4');")
            .arg(TableClientsName)
            .arg(nick)
            .arg(password)
            .arg(inf);
    if(query.exec(str))
    {
        msg = "New client added to DB";
        emit logMessage(msg);
        return true;
    }
    else
    {
        msg = query.lastError().text();
        emit logMessage(msg);
        return false;
    }

}

bool DBManager::addNewChannel(QString name, QString topic)
{
    QString msg;
    QSqlQuery query;
    QString str = QString("INSERT INTO %1 (name, topic)"
            "VALUES ('%2', '%3');")
            .arg(TableChannelsName)
            .arg(name)
            .arg(topic);
    if(query.exec(str))
    {
        msg = "New channel added to DB";
        emit logMessage(msg);
        return true;
    }
    else
    {
        msg = query.lastError().text();
        emit logMessage(msg);
        return false;
    }
}

bool DBManager::isClient(QString nick)
{
    QSqlQuery query;
    QString msg;
    QString str = QString("SELECT * FROM %1 WHERE nick = %2;")
            .arg(TableClientsName)
            .arg("'" + nick + "'");
    if(!query.exec(str))
    {
        msg = query.lastError().text();
        emit logMessage(msg);
        return false;
    }
    else
        return (query.next)
}

void DBManager::editInf(QString nick, QString inf)
{
    QString msg;
    QSqlQuery query;
    QString str = QString("UPDATE %1 SET inf = %2 WHERE nick = %3;")
            .arg(TableClientsName)
            .arg("'" + inf + "'")
            .arg("'" + nick + "'");
    if(!query.exec(str))
    {
        msg = query.lastError().text();
        emit logMessage(msg);
    }
}

void DBManager::editPass(QString nick, QString password)
{
    QString msg;
    QSqlQuery query;
    QString str = QString("UPDATE %1 SET password = %2 WHERE nick = %3;")
            .arg(TableClientsName)
            .arg("'" + password + "'")
            .arg("'" + nick + "'");
    if(!query.exec(str))
    {
        msg = query.lastError().text();
        emit logMessage(msg);
    }
}

void DBManager::editTopic(QString channelname, QString topic)
{
    QString msg;
    QSqlQuery query;
    QString str = QString("UPDATE %1 SET topic = %2 WHERE name = %3;")
            .arg(TableChannelsName)
            .arg("'" + topic + "'")
            .arg("'" + channelname + "'");
    if(!query.exec(str))
    {
        msg = query.lastError().text();
        emit logMessage(msg);
    }
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

void DBManager::createDatabase()
{
    if (!createClientsTable() ||
        !createChannelsTable() ||
        !createMembershipTable())
        emit logMessage("Error creating new tables");
    ChatChannel channel;
    channel.setName("Main");
    channel.setDescription("Main channel, contains all users");
    channel.setTopic("Gimme fuj, gimme fai, gimme dabajabazza");
    setChannel(channel);
}

ChatClient *DBManager::getClient(QString username)
{
    ChatClient *client = NULL;
    QSqlQuery query;
    QString str = QString("SELECT * FROM %1 WHERE NAME = %2;")
                    .arg(m_clientTableName)
                    .arg(username);
    if (!query.exec(str) || !query.next())
    {
        emit logMessage(query.lastError().text());
    }
    else
    {
        // create new client and fill it with results of query
        client = new ChatClient;
        //blah-blah-blah
    }
}

void DBManager::setClient(ChatClient &client)
{
}

ChatChannel *DBManager::getChannel(QString channelName)
{
}

void DBManager::setChannel(ChatChannel &channel)
{
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

void GeneralClientList::authorizeClientInList(const QString &username, QTcpSocket *socket)
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

void GeneralClientList::clientDisconnected(QString &username)
{
    //remove client from list
}
