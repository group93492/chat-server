#include "ChatServer.h"

ChatServer::ChatServer(QObject *parent) :
    QObject(parent),
    m_nextBlockSize(0)
{
}

bool ChatServer::startServer(const quint16 nPort = defaultPort)
//start server
//initialize client list
{
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(serverGotNewConnection()));
    //create client list, load this bastard from db
    //clientList = GeneralClientList;
    connect(&clientList, SIGNAL(logMessage(QString&)), this, SLOT(replyLog(QString&)));
    clientList.readChannelsFromDB();
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
                processMessage(msg);
                delete msg;
                break;
            }
        case cmtAuthorizationRequest:
            {
                AuthorizationRequest *msg = new AuthorizationRequest(input);
                processMessage(msg, pClientSocket);
                delete msg;
                break;
            }
        case cmtDisconnectMessage:
            {
                DisconnectMessage *msg = new DisconnectMessage(input);
                processMessage(msg);
                delete msg;
                break;
            }
        case cmtRegistrationRequest:
            {
                RegistrationRequest *msg = new RegistrationRequest(input);
                processMessage(msg, pClientSocket);
                delete msg;
                break;
            }
        default:
            {
                qDebug() << "Server received unknown-typed message" << msgType;
                /*m_nextBlockSize = 0;
                return;*/
            }
        }
        m_nextBlockSize = 0;
    }
}

void ChatServer::replyLog(QString &str)
{
    emit logMessage(str);
}

void ChatServer::processMessage(ChannelMessage *msg)
//processing channel message
//we need to reply this message to all clients in channel,
{
    //got channel message
    //need to reply it to all authorized clients in that channel
    if (!msg)
    {
        qDebug() << "Error processing channel message - message is empty";
        return;
    }
    qDebug() << "Server processing channel message:" << msg->sender << msg->receiver << msg->messageText;
    QString messageText = QString("Received channel message. Sender: %1. Receiver: %2. Body: %3")
            .arg(msg->sender)
            .arg(msg->receiver)
            .arg(msg->messageText);
    emit logMessage(messageText);
    //and reply to all of receiver channel users a channel message
    sendMessageToChannel(msg->receiver, msg);
}

void ChatServer::processMessage(AuthorizationRequest *msg, QTcpSocket *socket)
//processing authorization request
//here we need to check whether user exists in table or not
//YOBA ETO YA, PSHH-PSSHHH
//and we need to form authorization answer and send it to client
{
    if (!msg)
    {
        qDebug() << "Error processing authprization request- message is empty";
        return;
    }
    qDebug() << "Server processing authorization request: " << msg->username << msg->password;
    AuthorizationAnswer *answer = new AuthorizationAnswer();
    switch (clientList.authorize(msg->username, msg->password, socket))
    {
    case GeneralClientList::arAllreadyAuthorized:
        {
            answer->authorizationResult = false;
            answer->denialReason = "User allready authorized";
            break;
        }
    case GeneralClientList::arWrongAuthData:
        {
            answer->authorizationResult = false;
            answer->denialReason = "Wrong username or password";
            break;
        }
    case GeneralClientList::arAuthSuccess:
        {
            answer->authorizationResult = true;
            break;
        }
    }
    sendMessageToClient(msg->username, answer);
    if (answer->authorizationResult)
    {
        ChannelListMessage *channelsMsg = new ChannelListMessage();
        channelsMsg->channelList = clientList.getChannelsForClient(msg->username);
        channelsMsg->listType = ChannelListMessage::listOfJoined;
        sendMessageToClient(msg->username, channelsMsg);
        delete channelsMsg;
    }
    delete answer;
}

void ChatServer::processMessage(DisconnectMessage *msg)
//processing disconnect message from client
//that's simple
//we need only to delete client from client list
//and close his socket

//also we should reply disconnect fact in all client's channels
//TODO:
{
    if (!msg)
    {
        qDebug() << "Error processing disconnect message - message is empty";
        return;
    }
    qDebug() << "Server processing disconnect message from:" << msg->sender;
    QString messageText = msg->sender + " was disconnected from server";
    emit logMessage(messageText);
    QStringList channels = clientList.getChannelsForClient(msg->sender);
    for (int i = 0; i < channels.count(); ++i)
        sendMessageToChannel(channels[i], msg);
}

void ChatServer::processMessage(RegistrationRequest *msg, QTcpSocket *socket)
//processing regisration message
//registration logic stored in clientList,
//we just call it and use results to send registration answer
{
    if (!msg)
    {
        qDebug() << "Error processing registration request - message is empty";
        return;
    }
    qDebug() << "Server processing registration request:" << msg->username << msg->password;
    RegistrationAnswer *answer = new RegistrationAnswer();
    switch (clientList.registrate(msg->username, msg->password))
    {
    case GeneralClientList::rrOccupiedUsername:
        {
            answer->registrationResult = false;
            answer->denialReason = "Username allready reserved, please choose another";
            break;
        }
    case GeneralClientList::rrBadUsername:
        {
            answer->registrationResult = false;
            answer->denialReason = "Your username isn't allowed";
            break;
        }
    case GeneralClientList::rrRegSuccess:
        {
            answer->registrationResult = true;
            break;
        }
    }
    sendMessageToClient(socket, answer);
    //
    qDebug() << msg->username << ((answer->registrationResult) ? " was registered." : "wasn't registered");
    //
    delete answer;
}

void ChatServer::sendMessageToClient(QString username, ChatMessageBody *msgBody)
//packing and sending allready formed message to authorized client
{
    QTcpSocket *socket = clientList.getClient(username).userSocket();
    QByteArray arrBlock;
    QDataStream output(&arrBlock, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_4_7);
    output << quint16(0);

//    ChatMessageSerializer::packMessage(output, msgBody);
    ChatMessageHeader *header = new ChatMessageHeader(msgBody);
    header->pack(output);
    msgBody->pack(output);
    delete header;

    output.device()->seek(0);
    output << quint16(arrBlock.size() - sizeof(quint16));
    socket->write(arrBlock);
}

void ChatServer::sendMessageToClient(QTcpSocket *socket, ChatMessageBody *msgBody)
//send message to client by his socket
//needed to send messages to non-authorized clients
//for example - registration answer
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

    output.device()->seek(0);
    output << quint16(arrBlock.size() - sizeof(quint16));
    socket->write(arrBlock);
}

void ChatServer::sendMessageToChannel(QString channelName, ChatMessageBody *msgBody)
//send message *msgBody to all clients, which are in channel <channelName>
//needed for replying channel and other messages for all channel
{
    if (!clientList.hasChannel(channelName))
        return;
    /*QByteArray arrBlock;
    QDataStream output(&arrBlock, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_4_7);
    output << quint16(0);
    ChatMessageHeader *header = new ChatMessageHeader(msgBody);
    header->pack(output);
    msgBody->pack(output);
    delete header;
    output << quint16(arrBlock.size() - sizeof(quint16));*/

    ChatChannel channel = clientList.getChannel(channelName);
    for (int i = 0; i < channel.userList.count(); i++)
    {
        sendMessageToClient(channel.userList[i], msgBody);
        /*channel.userList[i]->userSocket()->write(arrBlock);*/
    }
}
