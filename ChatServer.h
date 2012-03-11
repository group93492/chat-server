#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include "ChatMessages.h"
#include "configmanager.h"

class ChatServer : public QObject
{
    Q_OBJECT
private:
    QTcpServer *m_tcpServer;
    quint16 m_nextBlockSize;
    quint16 nPort;
    QMap<QString, QTcpSocket *> m_clientList;

    void processMessage(QTcpSocket *socket, ChannelMessage *msg);
    void processMessage(QTcpSocket *socket, AuthorizationRequest *msg);

    void sendMessageToClient(QTcpSocket *socket, ChatMessageBody* msgBody);

public:
    explicit ChatServer(QObject *parent = 0);
    bool startServer();

signals:
    void logMessage(QString &msg);

private slots:
    void serverGotNewConnection();
    void serverGotNewMessage();
public slots:
    void setConfig(ChatServerConfig *pointer);
};

#endif // CHATSERVER_H
