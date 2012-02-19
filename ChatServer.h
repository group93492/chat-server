#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <ChatMessageSerializer.h>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

class ChatServer : public QObject
{
    Q_OBJECT
private:
    QTcpServer *tcpServer;
    quint16 nextBlockSize;
    QMap<QString, QTcpSocket *> clientList;
    quint16 defaultServerPort;

    void processMessage(QTcpSocket *socket, InformationalMessage *msg);
    void processMessage(QTcpSocket *socket, AuthorizationRequest *msg);

    void sendMessageToClient(QTcpSocket *socket, ChatMessageBody* msgBody);
public:
    explicit ChatServer(QObject *parent = 0);
    bool startServer(quint16 nPort);
signals:
    void logMessage(QString &msg);
public slots:
    void serverGotNewConnection();
    void serverGotNewMessage();
};

#endif // CHATSERVER_H
