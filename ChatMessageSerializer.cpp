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
    ChatMessageType msgType = (ChatMessageType) header->getMessageType();
    ChatMessageBody *msgBody;
    delete header;
    switch(msgType)
    {
    case cmtChannelMessage:
        {
            msgBody = new ChannelMessage();
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
    header->setMessageType(msgBody->getMessageType());
    header->setMessageSize(sizeof(*msgBody));
    header->pack(msgStream);
    msgBody->pack(msgStream);
    delete header;
}


