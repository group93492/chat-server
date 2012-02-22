#include "ChatMessageSerializer.h"
#include <QDebug>

ChatMessageSerializer::ChatMessageSerializer(QObject *parent):
    QObject(parent)
{
}


ChatMessageBody *ChatMessageSerializer::unpackMessage(QDataStream &msgStream)
{
    ChatMessageHeader *header = new ChatMessageHeader();
    header->unpack(msgStream);
    ChatMessageType msgType = (ChatMessageType) header->messageType;
    ChatMessageBody *msgBody;
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
            return NULL;
        }
    default:
        {
            qDebug() << "Message processor found unknown message while unpacking";
            return NULL;
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


