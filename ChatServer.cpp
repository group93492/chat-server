#include "ChatServer.h"
using namespace std;
ChatServer::ChatServer(QObject *parent) :
    QObject(parent),
    defaultServerPort(33033),
    nextBlockSize(0)
{

}

bool ChatServer::startServer(quint16 nPort = 0)
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(serverGotNewConnection()));
    quint16 port = (nPort == 0) ? defaultServerPort : nPort;
    return tcpServer->listen(QHostAddress::Any, port);
}

void ChatServer::serverGotNewConnection()
{
    QTcpSocket *newSocket = tcpServer->nextPendingConnection();
    connect(newSocket, SIGNAL(readyRead()), this, SLOT(serverGotNewMessage()));
    qDebug() << "Server got new connection from" << newSocket->peerAddress() << newSocket->peerPort();
}

void ChatServer::serverGotNewMessage()
{
    qDebug() << "Server received new message";
    QTcpSocket *pClientSocket = (QTcpSocket*)sender();
    QDataStream input(pClientSocket);
    input.setVersion(QDataStream::Qt_4_7);
    while (true)
    {
        if(!nextBlockSize)
        {
            if(pClientSocket->bytesAvailable() < sizeof(quint16))
                break;
            input >> nextBlockSize;
        }
        if(pClientSocket->bytesAvailable() < nextBlockSize)
            break;
        ChatMessageBody *newMessage = ChatMessageSerializer::unpackMessage(input);
        //want something like this
        //processMessage(pClientSocket, newMessage);
        //but now could do only like this
        switch ((ChatMessageType) newMessage->messageType)
        {
        case cmtInformationalMessage:
            {
                processMessage(pClientSocket, (InformationalMessage *) newMessage);
                break;
            }
        case cmtAuthorizationRequest:
            {
                processMessage(pClientSocket, (AuthorizationRequest *) newMessage);
                break;
            }
        default:
            qDebug() << "Server received unknown type of message";
        }
        delete newMessage;
        nextBlockSize = 0;
    }
}

void ChatServer::processMessage(QTcpSocket *socket, InformationalMessage *msg)
{
    //got informational message
    //need to reply it to all authorized clients
    qDebug() << "Server processing informational message:" << msg->sender << msg->receiver << msg->messageBody;
    QString messageText = "Received informational message. Sender: %1. Receiver: %2. Body: %3";
    messageText.arg(msg->sender)
               .arg(msg->receiver)
               .arg(msg->messageBody);
    emit logMessage(messageText);
    QMap<QString, QTcpSocket *>::iterator it = clientList.begin();
    for (; it != clientList.end(); ++it)
    {
        sendMessageToClient(it.value(), msg);
    }
}

void ChatServer::processMessage(QTcpSocket *socket, AuthorizationRequest *msg)
{
    qDebug() << "Server processing authorization request: " << msg->username << msg->password;
    bool authResult = msg->username.contains("yoba", Qt::CaseInsensitive);
    QString messageText =   "Received authorization request from %1. He says that his name is %2 and password is %3."
                            "I think i should%4authorize him because %5.";
    messageText.arg(socket->peerAddress().toString())
               .arg(msg->username)
               .arg(msg->password)
               .arg((authResult) ? " ": " not ")
               .arg((authResult) ? "i like him" : "he's fool");
    AuthorizationAnswer *answer = new AuthorizationAnswer();
    answer->authorizationResult = authResult;
    if (authResult)
    {
        //add him to client list
        clientList.insert(msg->username, socket);
        //tell him, that he passed authorization
    }
    else
    {
        answer->authorizationReason = "Invalid username or password";
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
    ChatMessageSerializer::packMessage(output, msgBody);
    output << quint16(arrBlock.size() - sizeof(quint16));
    socket->write(arrBlock);
}
