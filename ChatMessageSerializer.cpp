#include "ChatMessageSerializer.h"
#include <QDebug>

ChatMessageSerializer::ChatMessageSerializer()
{
}

ChatMessageType ChatMessageSerializer::unpackMessage(QDataStream &msgStream, ChatMessageBody *msgBody)
{
}

ChatMessageBody *ChatMessageSerializer::unpackMessage(QDataStream &msgStream)
{
    ChatMessageHeader *header = new ChatMessageHeader();
    header->unpack(msgStream);
    ChatMessageType msgType = (ChatMessageType) header->messageType;
    ChatMessageBody *msgBody;
    msgBody->messageType = header->messageType;
    delete header;
    switch(msgType)
    {
    case cmtInformationalMessage:
        {
            msgBody = new InformationalMessage();
            break;
        }
    case cmtAuthorizationRequest:
        {
            msgBody = new AuthorizationRequest();
            break;
        }
    case cmtAuthorizationAnswer:
        {
            msgBody = new AuthorizationAnswer();
            break;
        }
    case cmtUnknown:
        {
            return msgType;
        }
    default:
        {
            qDebug() << "Message processor found unknown message while unpacking";
            return cmtUnknown;
        }
    }
    msgBody->unpack(msgStream);
    return msgBody;
}

void ChatMessageSerializer::packMessage(QDataStream &msgStream, ChatMessageBody *msgBody)
{
    ChatMessageHeader *header = new ChatMessageHeader();
    header->messageType = msgBody->messageType;
    header->messageSize = sizeof(*msgBody);
    header->pack(msgStream);
    msgBody->pack(msgStream);
    delete header;
}

/*
void ChatMessageProcessor::unpackMessage(QDataStream &messageStream)
{
    ChatMessageHeader *header = new ChatMessageHeader();
    header->unpack(messageStream);
    ChatMessageBody *msg;
    switch((ChatMessageType) header->messageType)
    {
    case cmtInformationalMessage:
        {
            msg = new InformationalMessage();
            break;
        }
    case cmtAuthorizationRequest:
        {
            msg = new AuthorizationRequest();
            break;
        }
    case cmtAuthorizationAnswer:
        {
            msg = new AuthorizationAnswer();
            break;
        }
    default:
        {
            qDebug() << "Message processor found unknown message while unpacking";
            return;
        }
    }
    delete header;
    msg->unpack(messageStream);
    emit messageUnpacked(msg);
}

void ChatMessageProcessor::packMessage(ChatMessageBody *msgBody, ChatMessageType msgType)
{
    ChatMessageHeader *header = new ChatMessageHeader();
    header->messageType = msgType;
    header->messageSize = sizeof(*msgBody);
    QDataStream msgStream;
    header->pack(msgStream);
    msgBody->pack(msgStream);
    emit messagePacked(msgStream);
}*/


