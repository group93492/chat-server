#include "logger.h"

Logger::Logger(QObject *parent) :
    QObject(parent)
{
    m_Dir = new QDir();
}

Logger::~Logger()
{
    QFile *temp;
    QMap<QString, QFile *>::iterator it = m_ListOfLogs.begin();
    for (; it != m_ListOfLogs.end(); ++it)
    {
        temp = it.value();
        temp->close();
        delete it.value();
    }
    delete m_Dir;
}

void Logger::AddToChannelLog(QString &ChannelName, QString &Message)
{
    if(m_currentDay != QDate::currentDate().day())
        changeFolder();
    QString outMsg;
    outMsg = "[" + QTime::currentTime().toString("hh:mm:ss") + "]";
    if(!m_ListOfLogs.contains(ChannelName))
    {
        QFile *file = new QFile(ChannelName + ".txt");
        if(file->exists())
        {
            if(!file->open(QIODevice::Append))
            {
                QString msg = "Unable open log file: " + file->fileName();
                emit logMessage(msg);
                return;
            }
        }
        else
        {
            if(!file->open(QIODevice::WriteOnly))
            {
                QString msg = "Unable open log file: " + file->fileName();
                emit logMessage(msg);
                return;
            }
        }
        m_ListOfLogs.insert(ChannelName, file);
    }
    QTextStream out(m_ListOfLogs.value(ChannelName));
    outMsg += Message + "\n";
    out << outMsg;
}

void Logger::AddToServerLog(ErrorStatus Status, QString &Message)
{
    if(m_currentDay != QDate::currentDate().day())
        changeFolder();
    QString outMsg;
    outMsg = "[" + QTime::currentTime().toString("hh:mm:ss") + "]";
    if(!m_ListOfLogs.contains("server"))
    {
        QFile *file = new QFile("server.txt");
        if(file->exists())
        {
            if(!file->open(QIODevice::Append))
            {
                QString msg = "Unable open server log file";
                emit logMessage(msg);
                return;
            }
        }
        else
        {
            if(!file->open(QIODevice::WriteOnly))
            {
                QString msg = "Unable open server log file";
                emit logMessage(msg);
                return;
            }
        }
        m_ListOfLogs.insert("server", file);
    }
    QTextStream out(m_ListOfLogs.value("server"));
    outMsg = outMsg + "[" + Status + "]" + Message; //FIXME: Status
    emit logMessage(outMsg);
    outMsg += "\n";
    out << outMsg;
}

void Logger::SetSettings(QString path)
{
    m_Path = path;
}

void Logger::StartLogger()
{
    QString currentDate;
    currentDate = QDate::currentDate().toString("dd.MM.yyyy");
    m_currentDay = QDate::currentDate().day();
    if(!m_Dir->cd(m_Path)) //create log directory
    {
        m_Dir->mkdir(m_Path);
        m_Dir->cd(m_Path);
    }
    else
    {
        m_Dir->cd(m_Path);
    }
    if(!m_Dir->cd(currentDate)) //create directory for specific date
    {
        m_Dir->mkdir(currentDate);
        m_Dir->cd(currentDate);
    }
    else
    {
        m_Dir->cd(currentDate);
    }
    QDir::setCurrent(m_Dir->absolutePath());
}

void Logger::changeFolder()
{
    m_ListOfLogs.clear();
    m_Dir->makeAbsolute();
    m_Dir->cdUp();
    QString currentDate;
    currentDate = QDate::currentDate().toString("dd.MM.yyyy");
    m_Dir->mkdir(currentDate);
    m_Dir->cd(currentDate);
    m_currentDay++;
    QDir::setCurrent(m_Dir->absolutePath());
}



