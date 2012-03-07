#ifndef USESDATABASE_H
#define USESDATABASE_H

#include <QObject>
#include "QtSql/QtSql"
#include <QTableView>

class DBManager: public QObject
{
    Q_OBJECT
private:
    QSqlDatabase DataBase;
    QString DataBaseName;
    QString TableClientsName;
    QString TableChannelsName;
    QTableView *tableView;
public:
    explicit DBManager(QObject *parent = 0);
    explicit DBManager(QString name);
    ~DBManager();
    void connectToBase();
    void disconnectBase();
    void createClientsTable();
    void createChannelsTable();
    bool addNewClient(QString, QString, QString inf);
    bool addNewChannel(QString, QString topic);
    bool isClient(QString nick);
    void editInf(QString, QString inf);
    void editPass(QString, QString password);
    void editTopic(QString channelname, QString);
    bool authorization(QString nick, QString);
    //

signals:
    void logMessage(QString&);
public slots:
    void lookTable(QTableView *, QString tablename);
};

#endif // USESDATABASE_H
