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
    explicit DBManager(QString);
    ~DBManager();
    void disconnectBase();
    void createClientsTable();
    void createChannelsTable();
    void connectToBase();
    bool addNewClient(QString, QString, QString);
    bool addNewChannel(QString, QString);
    bool isClient(QString);
    void editInf(QString, QString);
    void editPass(QString, QString);
    void editTopic(QString, QString);
    bool authorization(QString, QString);
signals:
    void logMessage(QString&);
public slots:
    void lookTable(QTableView *, QString);
};

#endif // USESDATABASE_H
