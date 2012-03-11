#include "ChatServer.h"

ChatServer::ChatServer(QObject *parent) :
    QObject(parent),
    m_nextBlockSize(0)
{
}

bool ChatServer::startServer()
{
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(serverGotNewConnection()));
    return m_tcpServer->listen(QHostAddress::Any, nPort);
}

void ChatServer::serverGotNewConnection()
{
    QTcpSocket *newSocket = m_tcpServer->nextPendingConnection();
    connect(newSocket, SIGNAL(readyRead()), this, SLOT(serverGotNewMessage()));
    qDebug() << "Server got new connection from" << newSocket->peerAddress().toString() << newSocket->peerPort();
}

void ChatServer::serverGotNewMessage()
{
    qDebug() << "Server received new message";
    QTcpSocket *pClientSocket = (QTcpSocket*)sender();
    QDataStream input(pClientSocket);
    input.setVersion(QDataStream::Qt_4_7);
    while (true)
    {
        if (!m_nextBlockSize)
        {
            if (pClientSocket->bytesAvailable() < sizeof(quint16))
                break;
            input >> m_nextBlockSize;
        }
        if (pClientSocket->bytesAvailable() < m_nextBlockSize)
            break;
        //message in in <input>, unpack it
        ChatMessageHeader *header = new ChatMessageHeader(input);
        ChatMessageType msgType = (ChatMessageType) header->messageType;
        delete header;
        switch (msgType)
        {
        case cmtChannelMessage:
            {
                ChannelMessage *msg = new ChannelMessage(input);
                processMessage(pClientSocket, msg);
                delete msg;
                break;
            }
        case cmtAuthorizationRequest:
            {
                AuthorizationRequest *msg = new AuthorizationRequest(input);
                processMessage(pClientSocket, msg);
                delete msg;
                break;
            }
        default:
            {
                qDebug() << "Server received unknown-typed message";
                return;
            }
        }
    }
    m_nextBlockSize = 0;
}

void ChatServer::setConfig(ChatServerConfig *pointer)
{
    nPort = pointer->port;
}

void ChatServer::processMessage(QTcpSocket *socket, ChannelMessage *msg)
{
    //got informational message
    //need to reply it to all authorized clients
    qDebug() << "Server processing channel message:" << msg->sender << msg->receiver << msg->messageText;
    QString messageText = QString("Received channel message. Sender: %1. Receiver: %2. Body: %3")
            .arg(msg->sender)
            .arg(msg->receiver)
            .arg(msg->messageText);
    emit logMessage(messageText);
    QMap<QString, QTcpSocket *>::iterator it = m_clientList.begin();
    for (; it != m_clientList.end(); ++it)
        sendMessageToClient(it.value(), msg);
}

void ChatServer::processMessage(QTcpSocket *socket, AuthorizationRequest *msg)
{
    qDebug() << "Server processing authorization request: " << msg->username << msg->password;
    bool authResult = msg->username.contains("yoba", Qt::CaseInsensitive);
    QString messageText = QString("Received authorization request from %1. He says that his name is %2 and password is %3."
                        " I think i should%4authorize him because %5.")
                .arg(socket->peerAddress().toString())
                .arg(msg->username)
                .arg(msg->password)
                .arg((authResult) ? " ": " not ")
                .arg((authResult) ? "i like his name" : "he's fool");
    AuthorizationAnswer *answer = new AuthorizationAnswer();
    answer->authorizationResult = authResult;
    if (authResult)
    {
        //add him to client list
        m_clientList.insert(msg->username, socket);
        emit logMessage(messageText);
        //tell him, that he passed authorization
    }
    else
    {
        answer->denialReason = "Invalid username or password";
        //tell him that he hasn't pass authorization
    }
    sendMessageToClient(socket, answer);
    delete answer;
}

void ChatServer::sendMessageToClient(QTcpSocket *socket, ChatMessageBody *msgBody)
{
    QByteArray arrBlock;
    QDataStream output(&arrBlock, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_4_7);
    output << quint16(0);

//    ChatMessageSerializer::packMessage(output, msgBody);
    ChatMessageHeader *header = new ChatMessageHeader(msgBody);
    header->pack(output);
    msgBody->pack(output);
    delete header;

    output << quint16(arrBlock.size() - sizeof(quint16));
    socket->write(arrBlock);
}
