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
    void logMessage(QString &msg);
public slots:
    void SetSettings(QString path);
    void StartLogger();
    void AddToChannelLog(QString &name, QString &message);
    void AddToServerLog(ErrorStatus, QString &message);
};

#endif // LOGGER_H
