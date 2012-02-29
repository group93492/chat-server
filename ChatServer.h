#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include "ChatMessages.h"

class ChatServer : public QObject
{
    Q_OBJECT
private:
    QTcpServer *m_tcpServer;
    quint16 m_nextBlockSize;
    QMap<QString, QTcpSocket *> m_clientList;

    void processMessage(QTcpSocket *socket, ChannelMessage *msg);
    void processMessage(QTcpSocket *socket, AuthorizationRequest *msg);
    void processMessage(QTcpSocket *socket, DisconnectMessage *msg);
    void sendMessageToClient(QTcpSocket *socket, ChatMessageBody* msgBody);

public:
    enum { defaultPort = 33033 };
    explicit ChatServer(QObject *parent = 0);
    bool startServer(const quint16 nPort);

signals:
    void logMessage(QString &msg);

private slots:
    void serverGotNewConnection();
    void serverGotNewMessage();
};

#endif // CHATSERVER_H
