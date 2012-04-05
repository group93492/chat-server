#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QMap>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QTime>
#include <QDate>
#include <QDebug>
#include "configmanager.h"

enum ErrorStatus{
    esNotify = 0,
    esMinor = 1,
    esWarning = 2,
    esCritical = 3,
    esFatal = 4
};

class Logger : public QObject
{
    Q_OBJECT
private:
    QString arrayOfES[5];
    QMap<QString, QFile *> m_ListOfLogs;
    QDir *m_Dir;
    QString m_Path;
    QString m_currentLog;
    quint8 m_currentDay;
    void changeFolder();
public:
    explicit Logger(QObject *parent = 0);
    ~Logger();
signals:
    void logMessage(QString &msg);
    void addToListOfLogs(QStringList list);
public slots:
    void SetSettings(LoggerConfig *p_LoggerConfig);
    void StartLogger();
    void AddToChannelLog(QString &name, QString &message);
    void AddToServerLog(ErrorStatus, QString &message);
    void currentLog(QString str);
};

#endif // LOGGER_H
