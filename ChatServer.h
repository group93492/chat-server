#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include "ChatMessages.h"
#include "ClientList.h"

class ChatServer : public QObject
{
    Q_OBJECT
private:
    QTcpServer *m_tcpServer;
    quint16 m_nextBlockSize;
    QMap<QString, QTcpSocket *> m_clientList;
    GeneralClientList clientList;

    void processMessage(ChannelMessage *msg);
    void processMessage(AuthorizationRequest *msg);
    void processMessage(DisconnectMessage *msg);
    void sendMessageToClient(QString username, ChatMessageBody *msgBody);
    void sendMessageToChannel(QString channelName, ChatMessageBody *msgBody);

public:
    enum { defaultPort = 33033 };
    explicit ChatServer(QObject *parent = 0);
    bool startServer(const quint16 nPort);
    DBManager *DataBase;
signals:
    void logMessage(QString &msg);

private slots:
    void serverGotNewConnection();
    void serverGotNewMessage();
public slots:
};

#endif // CHATSERVER_H
