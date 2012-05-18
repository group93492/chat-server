#include "ChatServer.h"

ChatServer::ChatServer(QObject *parent) :
    QObject(parent),
    m_tcpServer(NULL),
    m_nextBlockSize(0)
{
}

bool ChatServer::startServer(const quint16 nPort = defaultPort)
//start server
//initialize client list
{
    m_tcpServer = new QTcpServer(this);
    m_port = nPort;
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(serverGotNewConnection()));
    //create client list, load this bastard from db
    //clientList = GeneralClientList;
    connect(&m_clientList, SIGNAL(logMessage(ErrorStatus, QString&)), this, SLOT(replyLog(ErrorStatus, QString&)));
    m_clientList.readChannelsFromDB();
    return m_tcpServer->listen(QHostAddress::Any, nPort);
}

void ChatServer::stopServer(const QString &shutdownReason)
//stops server
//we need to send disconnect messages to all channels
{
    GeneralClientList::userSocketsList_t *userList = m_clientList.getAllSockets();
    GeneralClientList::userSocketsListIterator_t itr(*userList);
    ServerShutdownMessage msg;
    msg.shutdownReason = shutdownReason;
    while(itr.hasNext())
        sendMessageToClient(itr.next(), &msg);
    delete userList;
    m_clientList.disconnectAll();
    QString log = tr("Server stopped.");
    m_tcpServer->close();
    disconnect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(serverGotNewConnection()));
    disconnect(&m_clientList, SIGNAL(logMessage(ErrorStatus, QString&)), this, SLOT(replyLog(ErrorStatus, QString&)));
    emit serverLog(esNotify, log);
    delete m_tcpServer;
}

void ChatServer::serverGotNewConnection()
//activates when server got new incoming connection
//isn't very important for us, cause server requires authorization
//so we still waiting authorization/registration request from that son of a bitch
{
    QTcpSocket *newSocket = m_tcpServer->nextPendingConnection();
    connect(newSocket, SIGNAL(readyRead()), this, SLOT(serverGotNewMessage()));
    QString log = "Server got new connection from " + newSocket->peerAddress().toString() + ":" + QString::number(newSocket->peerPort());
    emit serverLog(esNotify, log);
}

void ChatServer::serverGotNewMessage()
//activates when server receives new message
//in this method we are reading new message from socket
//first we read header
//then we read body of the message and call method
//that could process message its[message body] type
{
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
        case cmtChannelListRequest:
            {
                ChannelListRequest *msg = new ChannelListRequest(input);
                processMessage(msg, pClientSocket);
                delete msg;
                break;
            }
        case cmtChannelJoinRequest:
            {
                ChannelJoinRequest *msg = new ChannelJoinRequest(input);
                processMessage(msg, pClientSocket);
                delete msg;
                break;
            }
        case cmtChannelLeaveMessage:
            {
                ChannelLeaveMessage *msg = new ChannelLeaveMessage(input);
                processMessage(msg);
                delete msg;
                break;
            }
        case cmtChannelCreateRequest:
            {
                ChannelCreateRequest *msg = new ChannelCreateRequest(input);
                processMessage(msg);
                delete msg;
                break;
            }
        case cmtChannelThemeChanged:
            {
                ChannelThemeChanged  *msg = new ChannelThemeChanged(input);
                processMessage(msg);
                delete msg;
                break;
            }
        case cmtClientStatusChanged:
            {
                ClientStatusChanged *msg = new ClientStatusChanged(input);
                processMessage(msg);
                delete msg;
                break;
            }
        case cmtUserInfoRequest:
            {
                UserInfoRequest *msg = new UserInfoRequest(input);
                processMessage(msg, pClientSocket);
                delete msg;
                break;
            }
        case cmtUserInfoChanged:
            {
                UserInfoChanged *msg = new UserInfoChanged(input);
                processMessage(msg);
                delete msg;
                break;
            }
        case cmtPasswordChangeRequest:
            {
                PasswordChangeRequest *msg = new PasswordChangeRequest(input);
                processMessage(msg);
                delete msg;
                break;
            }
        default:
            {
                QString log = "Server received unknown-typed message" + msgType;
                emit serverLog(esWarning, log);
                break;
            }
        }
        m_nextBlockSize = 0;
    }
}

void ChatServer::replyLog(ErrorStatus status, QString &str)
{
    emit serverLog(status, str);
}

void ChatServer::setConfig(ChatServerConfig *pointer)
{
    m_port = pointer->port;
}

void ChatServer::processMessage(ChannelMessage *msg)
//processing channel message
//we need to reply this message to all clients in channel,
{
    //got channel message
    //need to reply it to all authorized clients in that channel
    if (!msg)
    {
        QString log = "Error processing channel message - message is empty";
        emit serverLog(esMinor, log);
        return;
    }
    QString messageText = QString("%1: %2")
            .arg(msg->sender)
            .arg(msg->messageText);
    emit channelLog(msg->receiver, messageText);
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
        QString log = "Error processing authprization request- message is empty";
        emit serverLog(esMinor, log);
        return;
    }
    AuthorizationAnswer *answer = new AuthorizationAnswer();
    switch (m_clientList.authorize(msg->username, msg->password, socket))
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
    sendMessageToClient(socket, answer);
    if (answer->authorizationResult)
    {
        ChatClient client = m_clientList.getClient(msg->username);
        client.setUserState("Online");
        m_clientList.updateClient(client);
        emit updateTable("clients");
        QStringList channels = m_clientList.getChannelsForClient(msg->username).keys();
        ChannelSystemMessage *informMsg = new ChannelSystemMessage();
        ChannelUserList *updateListMsg = new ChannelUserList();
        informMsg->message = msg->username + " entered chat.";
        for (int i = 0; i < channels.count(); ++i)
        {
            informMsg->channelName = channels[i];
            updateListMsg->channelName = channels[i];
            ChatChannel channel = m_clientList.getChannel(channels[i]);
            for(int k = 0; k < channel.userList.count(); k++)
            {
                updateListMsg->userList.insert(channel.userList[k], m_clientList.getClient(channel.userList[k]).userState());
            }
            for (int j = 0; j < channel.userList.count(); j++)
            {                
                QString username = channel.userList[j];
                if (username != msg->username)
                {
                    sendMessageToClient(username, informMsg);
                    sendMessageToClient(username, updateListMsg);
                }
            }
        }
        delete updateListMsg;
        delete informMsg;
    }
    delete answer;
}

void ChatServer::processMessage(DisconnectMessage *msg)
//processing disconnect message from client
//that's simple
//we need only to delete client from client list
//and close his socket
//also we should reply disconnect fact in all client's channels
{
    if (!msg)
    {
        QString log = "Error processing disconnect message - message is empty";
        emit serverLog(esMinor, log);
        return;
    }
    QString messageText = msg->sender + " was disconnected from server.";
    emit serverLog(esNotify, messageText);
    QStringList channels = m_clientList.getChannelsForClient(msg->sender).keys();
    ChannelSystemMessage *inform = new ChannelSystemMessage();
    ChannelUserList *list;
    inform->message = msg->sender+ " leaved chat.";
    m_clientList.disconnect(msg->sender);
    for (int i = 0; i < channels.count(); ++i)
    {
        list = new ChannelUserList();
        list->channelName = channels[i];
        ChatChannel channel = m_clientList.getChannel(channels[i]);
        for(int j = 0; j < channel.userList.count(); j++)
        {
            list->userList.insert(channel.userList[j], m_clientList.getClient(channel.userList[j]).userState());
        }
        sendMessageToChannel(channels[i], list);
        delete list;
        inform->channelName = channels[i];
        sendMessageToChannel(channels[i], inform);
    }
    emit updateTable("clients");
    delete inform;
}

void ChatServer::processMessage(RegistrationRequest *msg, QTcpSocket *socket)
//processing regisration message
//registration logic stored in clientList,
//we just call it and use results to send registration answer
{
    if (!msg)
    {
        QString log = "Error processing registration request - message is empty";
        emit serverLog(esMinor, log);
        return;
    }
    RegistrationAnswer *answer = new RegistrationAnswer();
    switch (m_clientList.registrate(msg->username, msg->password))
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
            emit updateTable("clients");
            emit updateTable("membership");
            break;
        }
    }
    sendMessageToClient(socket, answer);
    //
    QString log = msg->username + ((answer->registrationResult) ? " was registered." : "wasn't registered");
    emit serverLog(esNotify, log);
    //
    delete answer;
}

void ChatServer::processMessage(ChannelListRequest *msg, QTcpSocket *socket)
//new processMessage for ChannelListRequest
{
    if (!msg)
    {
        QString log = "Error processing authorization request- message is empty";
        emit serverLog(esMinor, log);
        return;
    }
    ChannelListMessage *chanListMsg = new ChannelListMessage();
    if(msg->listType == ChannelListRequest::listOfAll)
    {
        chanListMsg->listType = ChannelListMessage::listOfAll;
        chanListMsg->channelList = m_clientList.getAllChanells();
        sendMessageToClient(socket, chanListMsg);
    }
    else
    {
        chanListMsg->listType = ChannelListMessage::listOfJoined;
        chanListMsg->channelList = m_clientList.getChannelsForClient(msg->nick);
        sendMessageToClient(socket, chanListMsg);
        ChannelUserList *userListMsg = new ChannelUserList();
        ChannelThemeChanged *theme = new ChannelThemeChanged();
        QMap<QString, QString>::iterator channel = chanListMsg->channelList.begin();
        for(;channel != chanListMsg->channelList.end(); ++channel)
        {
            QString channelName = channel.key();
            ChatChannel tempChannel = m_clientList.getChannel(channelName);
            userListMsg->channelName = channel.key();
            for(int i = 0; i < tempChannel.userList.count(); i++)
            {
                userListMsg->userList.insert(tempChannel.userList[i], m_clientList.getClient(tempChannel.userList[i]).userState());
            }
            theme->channel = channel.key();
            theme->theme = m_clientList.getChannel(userListMsg->channelName).topic();
            sendMessageToChannel(channel.key(), userListMsg);
            sendMessageToClient(msg->nick, theme);
        }
        delete userListMsg;
        delete theme;
        UserInfoMessage *userInfo = new UserInfoMessage();
        userInfo->username = msg->nick;
        userInfo->info = m_clientList.getClient(msg->nick).userInfo();
        sendMessageToClient(msg->nick, userInfo);
        delete userInfo;
    }
    delete chanListMsg;
}

void ChatServer::processMessage(ChannelJoinRequest *msg, QTcpSocket *socket)
{
    ChannelJoinResult *answer = new ChannelJoinResult();
    if(m_clientList.hasChannel(msg->channelName) && !m_clientList.getChannel(msg->channelName).hasClient(msg->nick))
    {
        m_clientList.joinChannel(msg->nick, msg->channelName);
        answer->result = true;
        ChannelListMessage *listUpdate = new ChannelListMessage();
        listUpdate->listType = ChannelListMessage::listOfJoined;
        listUpdate->channelList = m_clientList.getChannelsForClient(msg->nick);
        sendMessageToClient(msg->nick, listUpdate);
        delete listUpdate;
        ChannelSystemMessage *newmsg = new ChannelSystemMessage();
        newmsg->message = msg->nick + " joined channel";
        newmsg->channelName = msg->channelName;
        sendMessageToChannel(msg->channelName, newmsg);
        delete newmsg;
        emit updateTable("membership");
        ChannelUserList *list = new ChannelUserList();
        list->channelName = msg->channelName;
        ChatChannel tempChannel = m_clientList.getChannel(msg->channelName);
        for(int i = 0; i < tempChannel.userList.count(); i++)
        {
            list->userList.insert(tempChannel.userList[i], m_clientList.getClient(tempChannel.userList[i]).userState());
        }
        sendMessageToChannel(msg->channelName, list);
        delete list;
        ChannelThemeChanged *theme = new ChannelThemeChanged();
        theme->channel = msg->channelName;
        theme->theme = m_clientList.getChannel(msg->channelName).topic();
        sendMessageToClient(msg->nick, theme);
        delete theme;
    }
    else
    {
        answer->result = false;
    }
    answer->channelName = msg->channelName;
    sendMessageToClient(socket, answer);
    delete answer;
}

void ChatServer::processMessage(ChannelLeaveMessage *msg)
{
    if(m_clientList.getChannel(msg->channelName).hasClient(msg->nick))
    {
        //deleting user from channel
        m_clientList.leaveChannel(msg->nick, msg->channelName);
        //send message "user <tadada> left channel" to all remaining users in channel
        ChannelSystemMessage *notifyMsg = new ChannelSystemMessage();
        notifyMsg->message = msg->nick + " left channel";
        notifyMsg->channelName = msg->channelName;
        sendMessageToChannel(msg->channelName, notifyMsg);
        //write a log about it
        channelLog(msg->channelName, notifyMsg->message);
        delete notifyMsg;
        //update membership table
        emit updateTable("membership");
        //update userlists of all remaining users in channel
        ChannelUserList *userListUpdateMsg = new ChannelUserList();
        userListUpdateMsg->channelName = msg->channelName;
        ChatChannel tempChannel = m_clientList.getChannel(msg->channelName);
        for(int i = 0; i < tempChannel.userList.count(); i++)
        {
            userListUpdateMsg->userList.insert(tempChannel.userList[i], m_clientList.getClient(tempChannel.userList[i]).userState());
        }
        sendMessageToChannel(msg->channelName, userListUpdateMsg);
        delete userListUpdateMsg;
    }
}

void ChatServer::processMessage(ChannelCreateRequest *msg)
{
    ChannelCreateResult *result = new ChannelCreateResult();
    switch (m_clientList.createChannel(msg->channelName,
                                     msg->channelDescription,
                                     msg->channelTopic))
    {
    case GeneralClientList::ccrTooManyChannels:
        {
            result->answer = false;
            result->denialReason = "Too many channels already created on server.";
        }
    case GeneralClientList::ccrBadName:
        {
            result->answer = false;
            result->denialReason = "Name of channel is already reserved";
        }
    case GeneralClientList::ccrSuccess:
        {
            result->answer = true;
        }
    }
    sendMessageToClient(msg->username, result);
    if (result->answer)
    {
        m_clientList.joinChannel(msg->username, msg->channelName);
        ChannelListMessage *userListUpdate = new ChannelListMessage();
        userListUpdate->listType = ChannelListMessage::listOfJoined;
        userListUpdate->channelList = m_clientList.getChannelsForClient(msg->username);
        sendMessageToClient(msg->username, userListUpdate);
        delete userListUpdate;
        ChannelListMessage *listUpdate = new ChannelListMessage();
        listUpdate->listType = ChannelListMessage::listOfAll;
        listUpdate->channelList = m_clientList.getAllChanells();
        sendMessageToChannel("main", listUpdate);
        delete listUpdate;
        ChannelUserList *list = new ChannelUserList();
        list->channelName = msg->channelName;
        ChatChannel tempChannel = m_clientList.getChannel(msg->channelName);
        for(int i = 0; i < tempChannel.userList.count(); i++)
        {
            list->userList.insert(tempChannel.userList[i], m_clientList.getClient(tempChannel.userList[i]).userState());
        }
        sendMessageToChannel(msg->channelName, list);
        delete list;
        ChannelThemeChanged *theme = new ChannelThemeChanged();
        theme->channel = msg->channelName;
        theme->theme = m_clientList.getChannel(msg->channelName).topic();
        sendMessageToClient(msg->username, theme);
        delete theme;
        emit updateTable("channels");
        emit updateTable("membership");
    }
    delete result;
}

void ChatServer::processMessage(ChannelThemeChanged *msg)
{
    QString channelName = msg->channel;
    QString theme = msg->theme;
    QString username = msg->username;
    ChatChannel channel = m_clientList.getChannel(channelName);
    channel.setTopic(theme);
    m_clientList.updateChannel(channel);
    sendMessageToChannel(channelName, msg);
    ChannelSystemMessage *newmsg = new ChannelSystemMessage();
    newmsg->channelName = channelName;
    newmsg->message = QString("%1 changed channel theme to \"%2\"")
                              .arg(username)
                              .arg(theme);
    emit channelLog(channelName, newmsg->message);
    emit updateTable("channels");
    sendMessageToChannel(channelName, newmsg);
    delete newmsg;
}

void ChatServer::processMessage(ClientStatusChanged *msg)
{
    QStringList list = m_clientList.getChannelsForClient(msg->username).keys();
    QStringList::iterator itr = list.begin();
    ChannelSystemMessage *system = new ChannelSystemMessage();
    ChatClient client = m_clientList.getClient(msg->username);
    if(msg->status.isEmpty())
    {
        system->message = msg->username + " returned to original state";
        client.setUserState("Online");
    }
    else
    {
        client.setUserState(msg->status);
        system->message = QString("%1 changed his state to \"%2\"")
                                  .arg(msg->username)
                                  .arg(msg->status);
    }
    m_clientList.updateClient(client);
    emit updateTable("clients");
    for(; itr != list.end(); ++itr)
    {
        system->channelName = *itr;
        sendMessageToChannel(*itr, msg);
        sendMessageToChannel(*itr, system);
        emit channelLog(*itr, system->message);
    }
    delete system;
}

void ChatServer::processMessage(UserInfoRequest *msg, QTcpSocket *socket)
{
    UserInfoMessage *answer = new UserInfoMessage();
    answer->username = msg->username;
    answer->info = m_clientList.getClient(msg->username).userInfo();
    sendMessageToClient(socket, answer);
    delete answer;
}

void ChatServer::processMessage(UserInfoChanged *msg)
{
    ChatClient client = m_clientList.getClient(msg->username);
    client.setUserInfo(msg->info);
    m_clientList.updateClient(client);
    QString log = "User " + msg->username + " changed his info";
    emit updateTable("clients");
    emit serverLog(esNotify, log);
}

void ChatServer::processMessage(PasswordChangeRequest *msg)
{
    PasswordChangeResult *answer = new PasswordChangeResult;
    ChatClient client = m_clientList.getClient(msg->username);
    QString log;
    if(client.password() == msg->oldPassword)
    {
        client.setPassword(msg->newPassword);
        m_clientList.updateClient(client);
        answer->result = "Password changed!\nDon't forget to use new password in the next authorization.";
        log = "User " + msg->username + " changed his password";
        emit updateTable("clients");
    }
    else
    {
        answer->result = "Given old password is incorrect!";
        log = "User " + msg->username + " tried to change his password. Unfortunately, unsuccessfull.";
    }
    emit serverLog(esNotify, log);
    sendMessageToClient(msg->username, answer);
}

void ChatServer::sendMessageToClient(QString username, ChatMessageBody *msgBody)
//packing and sending allready formed message to authorized client
{
    QTcpSocket *socket = m_clientList.getClient(username).userSocket();
    QByteArray arrBlock;
    QDataStream output(&arrBlock, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_4_7);
    output << quint16(0);

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
    if (!m_clientList.hasChannel(channelName))
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

    ChatChannel channel = m_clientList.getChannel(channelName);
    for (int i = 0; i < channel.userList.count(); i++)
    {
        sendMessageToClient(channel.userList[i], msgBody);
        /*channel.userList[i]->userSocket()->write(arrBlock);*/
    }
}
