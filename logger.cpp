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

void Logger::AddToChannelLog(QString ChannelName, QString Message)
{
    if(!m_ListOfLogs.contains(ChannelName))
    {
        QFile *file = new QFile(ChannelName + ".txt");
        if(file->exists())
        {
            if(!file->open(QIODevice::Append))
            {
                QString msg = "Unable open log file: " + file->fileName();
                emit logMessage(msg);
            }
        }
        else
        {
            if(!file->open(QIODevice::WriteOnly))
            {
                QString msg = "Unable open log file: " + file->fileName();
                emit logMessage(msg);
            }
        }
        m_ListOfLogs.insert(ChannelName, file);
        QTextStream out(m_ListOfLogs.value(ChannelName));
        out << Message << "\n";
    }
    else
    {
        QTextStream out(m_ListOfLogs.value(ChannelName));
        out << Message << "\n";
    }
}

void Logger::AddToServerLog(ErrorStatus Status, QString Message)
{
    if(!m_ListOfLogs.contains("server"))
    {
        QFile *file = new QFile("server.txt");
        if(file->exists())
        {
            if(!file->open(QIODevice::Append))
            {
                QString msg = "Unable open server log file";
                emit logMessage(msg);
            }
        }
        else
        {
            if(!file->open(QIODevice::WriteOnly))
            {
                QString msg = "Unable open server log file";
                emit logMessage(msg);
            }
        }
        m_ListOfLogs.insert("server", file);
        QTextStream out(m_ListOfLogs.value("server"));
        out << "[" << Status << "]" << Message << "\n";
    }
    else
    {
        QTextStream out(m_ListOfLogs.value("server"));
        out << "[" << Status << "]" << Message << "\n";
    }
}

void Logger::SetSettings(QString path)
{
    m_Path = path;
}

void Logger::StartLogger()
{
    if(!m_Dir->cd(m_Path))
    {
        m_Dir->mkdir(m_Path);
        m_Dir->cd(m_Path);
        QDir::setCurrent(m_Path);
    }
    else
    {
        m_Dir->cd(m_Path);
        QDir::setCurrent(m_Path);
    }
}
