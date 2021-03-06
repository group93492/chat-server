#include "DBManager.h"
#include <qtablewidget.h>

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
        {
            msg = DataBase.lastError().text();
            emit logMessage(msg);
        }
        else
        {
            msg = "DataBase found";
            emit logMessage(msg);
        }
    }
    else
    {
        DataBase.setDatabaseName(DataBaseName);
        if(!DataBase.open())
        {
            msg = DataBase.lastError().text();
            emit logMessage(msg);
        }
        else
        {
            msg = "DataBase not found, create new DataBase";
            emit logMessage(msg);
        }
        createClientsTable();
        createChannelsTable();
    }
}

void DBManager::disconnectBase()
{
    tableView->close();
    DataBase.close();
}

void DBManager::createClientsTable()
{
    QString msg;
    QSqlQuery query;
    QString str = "CREATE TABLE " + TableClientsName + " ( "
            "nick       VARCHAR(25) , "
            "password   VARCHAR(20), "
            "inf        VARCHAR(200) "
            ");";
    if(!query.exec(str))
    {
        msg = "Unable to create clients DB:" + query.lastError().text();
        emit logMessage(msg);
    }
    else
    {
        msg = "Clients table create";
        emit logMessage(msg);
    }
}

void DBManager::createChannelsTable()
{
    QString msg;
    QSqlQuery query;
    QString str = "CREATE TABLE " + TableChannelsName + " ( "
            "name    VARCHAR(25) , "
            "topic   VARCHAR(20) "
            ");";
    if(!query.exec(str))
    {
        msg = "Unable to create clients DB:" + query.lastError().text();
        emit logMessage(msg);
    }
    else
    {
        msg = "Channels table create";
        emit logMessage(msg);
        addNewChannel("main", "WELCOME!");
    }
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
        if(query.next())
            return true;
        else
            return false;
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


