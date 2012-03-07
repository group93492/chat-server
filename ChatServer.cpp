#include "ChatServer.h"

ChatServer::ChatServer(QObject *parent) :
    QObject(parent),
    m_nextBlockSize(0)
{
}

bool ChatServer::startServer(const quint16 nPort = defaultPort)
//start server
{
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(serverGotNewConnection()));
    return m_tcpServer->listen(QHostAddress::Any, nPort);
}

void ChatServer::serverGotNewConnection()
//activates when server got new incoming connection
//isn't very important for us, cause server requires authorization
//so we still waiting authorization/registration request from that son of a bitch
{
    QTcpSocket *newSocket = m_tcpServer->nextPendingConnection();
    connect(newSocket, SIGNAL(readyRead()), this, SLOT(serverGotNewMessage()));
    qDebug() << "Server got new connection from" << newSocket->peerAddress().toString() << newSocket->peerPort();
}

void ChatServer::serverGotNewMessage()
//activates when server receives new message
//in this method we are reading new message from socket
//first we read header
//then we read body of the message and call method
//that could process message its[message body] type
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
        case cmtDisconnectMessage:
            {
                DisconnectMessage *msg = new DisconnectMessage(input);
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

void ChatServer::processMessage(QTcpSocket *socket, ChannelMessage *msg)
//processing channel message
//we need to reply this message to all clients in channel,
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
//processing authprization request
//here we need to check whether user exists in table or not
//YOBA ETO YA, PSHH-PSSHHH
//and we need to form authorization answer and send it to client
{
    qDebug() << "Server processing authorization request: " << msg->username << msg->password;
    bool authResult;
    QString messageText;
    AuthorizationAnswer *answer = new AuthorizationAnswer();
    if(m_clientList.contains(msg->username))
    {
        answer->denialReason = QString("User %1 allready authorized on server.").arg(msg->username);
        authResult = false;
    }
    else
    {
        authResult = true;
        messageText = QString("Received authorization request from %1. He says that his name is %2 and password is %3."
                            " I think i should%4authorize him because %5.")
                    .arg(socket->peerAddress().toString())
                    .arg(msg->username)
                    .arg(msg->password)
                    .arg((authResult) ? " ": " not ")
                    .arg((authResult) ? "i like his name" : "he's fool");
        //add him to client list
        m_clientList.insert(msg->username, socket);
        emit logMessage(messageText);
        //tell him, that he passed authorization
    }
    answer->authorizationResult = authResult;
    sendMessageToClient(socket, answer);
    delete answer;
}

void ChatServer::processMessage(QTcpSocket *socket, DisconnectMessage *msg)
//processing disconnect message from client
//that's simple
//we need only to delete client from client list
//and close his socket

//also we should reply disconnect fact in all client's channels
//TODO:^
{
    qDebug() << "Server processing disconnect message from:" << msg->sender;
    QString messageText = msg->sender + " disconnect from server";
    emit logMessage(messageText);
    QMap<QString, QTcpSocket *>::iterator it = m_clientList.begin();
    m_clientList.remove(msg->sender);
    for (; it != m_clientList.end(); ++it)
        sendMessageToClient(it.value(), msg);
    socket->close();
}

void ChatServer::sendMessageToClient(QTcpSocket *socket, ChatMessageBody *msgBody)
//packing and sending allready formed message to client
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
