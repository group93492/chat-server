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
    QVector<ChatClient &> userList;
    // should be private. but i couldn't find a nice way to provide
    // an interface to iterate all the clients in the channel
    ChatChannel() {}
    QString &name();
    void setName(QString name);
    QString &description();
    void setDescription(QString desc);
    QString &topic();
    void setTopic(QString topic);
    void addClient(ChatClient &clnt);
    void deleteClient(ChatClient &clnt);
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
    ChatClient *getClient(QString username);
    void setClient(ChatClient &client);
    // channel table
    ChatChannel *getChannel(QString channelName);
    void setChannel(ChatChannel &channel);
    //membership table
    bool isMembership(QString username, QString channelName);
    void addMembership(QString username, QString channelName);
    //methods for receiving lists
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
    void readChannelsFromDB();
public:
    enum AuthResult
    {
        arAllreadyAuthorized,
        arAuthSuccess,
        arWrongAuthData
    };
    explicit GeneralClientList(QObject *parent = 0);
    ChatChannel &getChannel(const QString &channelName);
    ChatClient &getClient(const QString &username);
    bool hasClient(QString username);
    void addClient(const QString &username, QTcpSocket *socket);
    void removeClient(QString &username);
    QStringList &getChannelsForClient(QString username);

    AuthResult authorize(QString username, QString password);
    void disconnect(QString username);
    void joinChannel(QString username, QString channelName);
    void leaveChannel(QString username, QString channelName);
};

#endif // CLIENTLIST_H
