#ifndef CLIENTLIST_H
#define CLIENTLIST_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtNetwork/QTcpSocket>
#include <QtSql>
#include <QTableView>
/*
TODO now:
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
- private channels
*/
class ChatClient
{
private:
    QString m_username;
    QString m_userInfo;
    QTcpSocket *m_userSocket;
public:
    ChatClient(): m_userSocket(NULL) {}
    QString &username();
    void setUsername(QString name);
    QString &userInfo();
    void setUserInfo(QString info);
    QTcpSocket *userSocket() const;
    void setUserSocket(QTcpSocket *socket);
};

class ChatChannel
{
private:
    QString m_name;
    QString m_description;
    QString m_topic;
    QVector<ChatClient *> m_usrlist;
public:
    ChatChannel() {}
    QString &name();
    void setName(QString name);
    QString &description();
    void setDescription(QString desc);
    QString &topic();
    void setTopic(QString topic);
    void addClient(ChatClient *clnt);
    void deleteClient(ChatClient *clnt);
    QStringList getClientUsernameList();
//    QVectorIterator<ConnectedClient *> &getIterator();
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
    QTableView *tableView;
public:
    explicit DBManager(QObject *parent = 0);
    explicit DBManager(QString name);
    ~DBManager();
    void connectToBase();
    void disconnectBase();
    bool createClientsTable();
    bool createChannelsTable();
    bool createMembershipTable();
    bool addNewClient(QString, QString, QString inf);
    bool addNewChannel(QString, QString topic);
    bool isClient(QString nick);
    void editInf(QString, QString inf);
    void editPass(QString, QString password);
    void editTopic(QString channelname, QString);
    bool authorization(QString nick, QString);
    //general methods
    void createDatabase();
    // client table
    ChatClient *getClient(QString username);
    void setClient(ChatClient &client);
    // channel table
    ChatChannel *getChannel(QString channelName);
    void setChannel(ChatChannel &channel);
    //membership table
    bool isClientMemberOfChannel(QString username, QString channelName);
    void addMembership(QString username, QString channelName);

signals:
    void logMessage(QString&);
public slots:
    void lookTable(QTableView *, QString tablename);
};

class GeneralClientList: public QObject
{
Q_OBJECT
private:
    QVector<ChatClient> m_generalClientList;
    QMap<QString, ChatChannel> m_channelList;
public:
    explicit GeneralClientList(QObject *parent = 0): QObject(parent) {}
    void readChannelListFromDB();
    ChatChannel &getChannel(const QString &channelName);
public slots:
    void authorizeClientInList(const QString &username, QTcpSocket *socket);
    void clientDisconnected(QString &username);
};

#endif // CLIENTLIST_H
