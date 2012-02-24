#include "ChatServer.h"

ChatServer::ChatServer(QObject *parent) :
    QObject(parent),
    nextBlockSize(0)
{

}

bool ChatServer::startServer(quint16 nPort = 33033)
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(serverGotNewConnection()));
    return tcpServer->listen(QHostAddress::Any, nPort);
}

void ChatServer::serverGotNewConnection()
{
    QTcpSocket *newSocket = tcpServer->nextPendingConnection();
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
        if (!nextBlockSize)
        {
            if (pClientSocket->bytesAvailable() < sizeof(quint16))
                break;
            input >> nextBlockSize;
        }
        if (pClientSocket->bytesAvailable() < nextBlockSize)
            break;
        //message in in <input>, unpack it
        ChatMessageHeader *header = new ChatMessageHeader();
        header->unpack(input);
        ChatMessageType msgType = (ChatMessageType) header->getMessageType();
        delete header;
        switch (msgType)
        {
        case cmtChannelMessage:
            {
                ChannelMessage *msg = new ChannelMessage();
                msg->unpack(input);
                processMessage(pClientSocket, msg);
                delete msg;
                break;
            }
        case cmtAuthorizationRequest:
            {
                AuthorizationRequest *msg = new AuthorizationRequest();
                msg->unpack(input);
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
    nextBlockSize = 0;
}

void ChatServer::processMessage(QTcpSocket *socket, ChannelMessage *msg)
{
    //got informational message
    //need to reply it to all authorized clients
    qDebug() << "Server processing channel message:" << msg->getSender() << msg->getReceiver() << msg->getMessageText();
    QString messageText = QString("Received channel message. Sender: %1. Receiver: %2. Body: %3")
            .arg(msg->getSender())
            .arg(msg->getReceiver())
            .arg(msg->getMessageText());
    emit logMessage(messageText);
    QMap<QString, QTcpSocket *>::iterator it = clientList.begin();
    for (; it != clientList.end(); ++it)
    {
        sendMessageToClient(it.value(), msg);
    }
}

void ChatServer::processMessage(QTcpSocket *socket, AuthorizationRequest *msg)
{
    qDebug() << "Server processing authorization request: " << msg->getUsername() << msg->getPassword();
    bool authResult = msg->getUsername().contains("yoba", Qt::CaseInsensitive);
    QString messageText = QString("Received authorization request from %1. He says that his name is %2 and password is %3."
                        " I think i should%4authorize him because %5.")
                .arg(socket->peerAddress().toString())
                .arg(msg->getUsername())
                .arg(msg->getPassword())
                .arg((authResult) ? " ": " not ")
                .arg((authResult) ? "i like his name" : "he's fool");
    AuthorizationAnswer *answer = new AuthorizationAnswer();
    answer->setAuthorizationResult(authResult);
    if (authResult)
    {
        //add him to client list
        clientList.insert(msg->getUsername(), socket);
        emit logMessage(messageText);
        //tell him, that he passed authorization
    }
    else
    {
        answer->setDenialReason("Invalid username or password");
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
    ChatMessageHeader *header = new ChatMessageHeader();
    header->setMessageType(msgBody->getMessageType());
    header->setMessageSize(sizeof(*msgBody));
    header->pack(output);
    msgBody->pack(output);
    delete header;

    output << quint16(arrBlock.size() - sizeof(quint16));
    socket->write(arrBlock);
}
