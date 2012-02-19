#include "ChatMessageProcessor.h"

ChatMessageProcessor::ChatMessageProcessor(QObject *parent) :
    QObject(parent)
{
}

ChatMessageHeader &ChatMessageProcessor::getMessageHeader(QDataStream &msg)
{
    ChatMessageHeader header;
    header.unpack(msg);
    return header;
}

void ChatMessageProcessor::processMessage(QDataStream &messageStream)
{
    ChatMessageHeader *header = getMessageHeader(messageStream);
    ChatMessage *msg;
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
            qDebug() << "Message processor found unknown message";
            return;
        }
    }
    msg->unpack(messageStream);
}
