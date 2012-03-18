#ifndef CLIENTLIST_H
#define CLIENTLIST_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtNetwork/QTcpSocket>
#include <QtSql>
#include <QTableView>
#include "ChatMessages.h"
/*
TODO:
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
- leaving channels
- private channels
*/
class ChatClient
{
private:
    QString m_username;
    QString m_userInfo;
    QString m_userPassword;
    QTcpSocket *m_userSocket;
public:
    ChatClient(): m_userSocket(NULL) {}
    QString &username();
    void setUsername(QString name);
    QString &userInfo();
    void setUserInfo(QString info);
    QString &password();
    void setPassword(QString pass);
    QTcpSocket *userSocket() const;
    void setUserSocket(QTcpSocket *socket);
};

class ChatChannel
{
private:
    QString m_name;
    QString m_description;
    QString m_topic;
public:
    QStringList userList;
    // should be private. but i couldn't find a nice way to provide
    // an interface to iterate all the clients in the channel
    ChatChannel() {}
    QString &name();
    void setName(QString name);
    QString &description();
    void setDescription(QString desc);
    QString &topic();
    void setTopic(QString topic);
    void addClient(QString username);
    void deleteClient(QString username);
    bool hasClient(QString username);
};

class DBManager: public QObject
{
    Q_OBJECT
private:
    QSqlDatabase m_DB;
    QString m_DBName;
    const QString m_clientTableName;
    const QString m_channelTableName;
    const QString m_membershipTableName;
    const QString m_DBFileName;
    QTableView *tableView;
    bool createClientsTable();
    bool createChannelsTable();
    bool createMembershipTable();
public:
    explicit DBManager(QObject *parent = 0);
    ~DBManager();
    void connectToDB();
    void disconnectDB();
    //general methods
    void createDB();
    // client table
    bool hasClient(QString username);
    ChatClient getClient(QString username);
    void setClient(ChatClient &client);

    // channel table
    bool hasChannel(QString channelName);
    ChatChannel getChannel(QString channelName);
    void setChannel(ChatChannel &channel);
    //membership table
    bool isMembership(QString username, QString channelName);
    void addMembership(QString username, QString channelName);
    // methods for initializing channels in general client list
    QMap<QString, ChatChannel> getChannelList();
signals:
    void logMessage(QString&);
public slots:
    void lookTable(QTableView *, QString tablename);
};

class GeneralClientList: public QObject
{
Q_OBJECT
private:
    QMap<QString, ChatClient> m_generalClientList;
    QMap<QString, ChatChannel> m_channelList;
    DBManager m_DB;
public:
    enum AuthResult
    {
        arAllreadyAuthorized,
        arAuthSuccess,
        arWrongAuthData
    };
    enum RegResult
    {
        rrBadUsername,
        rrOccupiedUsername,
        rrRegSuccess
    };
    explicit GeneralClientList(QObject *parent = 0);
    void readChannelsFromDB();
    ChatChannel getChannel(QString &channelName);
    bool hasChannel(QString channelName);
    ChatClient getClient(const QString &username);
    bool hasClient(QString username);
    QStringList getChannelsForClient(QString username);

    RegResult registrate(QString username, QString password);
    AuthResult authorize(QString username, QString password, QTcpSocket *socket);
    void disconnect(QString username);
    void joinChannel(QString username, QString channelName);
    void leaveChannel(QString username, QString channelName);//

};

#endif // CLIENTLIST_H
