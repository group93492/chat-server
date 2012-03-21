#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QMap>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QTime>
#include <QDateTime>

enum ErrorStatus{
    esNotify,
    esMinor,
    esWarning,
    esCritical,
    esFatal
};

class Logger : public QObject
{
    Q_OBJECT
private:
    QMap<QString, QFile *> m_ListOfLogs;
    QDir *m_Dir;
    QString m_Path;
    quint8 m_currentDay;
    void changeFolder();
public:
    explicit Logger(QObject *parent = 0);
    ~Logger();
signals:
    void logMessage(QString &);
public slots:
    void SetSettings(QString);
    void StartLogger();
    void AddToChannelLog(QString, QString);
    void AddToServerLog(ErrorStatus, QString);
};

#endif // LOGGER_H
