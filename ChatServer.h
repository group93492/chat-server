#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include "ChatMessages.h"
#include "ClientList.h"
#include "configmanager.h"
#include "logger.h"

class ChatServer : public QObject
{
    Q_OBJECT
private:
    QTcpServer *m_tcpServer;
    quint16 m_nextBlockSize;
    quint16 m_port;
    GeneralClientList m_clientList;
    void processMessage(ChannelMessage *msg);
    void processMessage(AuthorizationRequest *msg, QTcpSocket *socket);
    void processMessage(DisconnectMessage *msg);
    void processMessage(RegistrationRequest *msg, QTcpSocket *socket);
    void processMessage(ChannelListRequest *msg, QTcpSocket *socket);
    void processMessage(ChannelJoinRequest *msg, QTcpSocket *socket);
    void processMessage(ChannelLeaveMessage *msg);
    void sendMessageToClient(QTcpSocket *socket, ChatMessageBody *msgBody);
    void sendMessageToClient(QString username, ChatMessageBody *msgBody);
    void sendMessageToChannel(QString channelName, ChatMessageBody *msgBody);

public:
    enum { defaultPort = 33033 };
    explicit ChatServer(QObject *parent = 0);
    bool startServer(const quint16 nPort);
    void stopServer();
signals:
    void serverLog(ErrorStatus, QString &message);
    void channelLog(QString &name, QString &message);

private slots:
    void serverGotNewConnection();
    void serverGotNewMessage();
    void replyLog(ErrorStatus status, QString &str);
public slots:
    void setConfig(ChatServerConfig *pointer);
};

#endif // CHATSERVER_H
