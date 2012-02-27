#include "ChatMessages.h"

ChatMessageHeader::ChatMessageHeader(const ChatMessageBody *msgBody)
{
    messageType = msgBody->messageType;
    messageSize = sizeof(*msgBody);
}

bool ChatMessageHeader::pack(QDataStream &stream) const
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream << messageType;
    return true;
}

bool ChatMessageHeader::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream >> messageType;
    return true;
}

AuthorizationAnswer::AuthorizationAnswer()
{
    messageType = cmtAuthorizationAnswer;
}

bool AuthorizationAnswer::pack(QDataStream &stream) const
{
    if (stream.status() != QDataStream::Ok)
        return false;
    if(authorizationResult)
        stream << authorizationResult;
    else
        stream << authorizationResult << denialReason;
    return true;
}

bool AuthorizationAnswer::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream >> authorizationResult;
    if(!authorizationResult)
        stream >> denialReason;
    return true;
}

AuthorizationRequest::AuthorizationRequest()
{
    messageType = cmtAuthorizationRequest;
}

bool AuthorizationRequest::pack(QDataStream &stream) const
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream << username << password;
    return true;
}

bool AuthorizationRequest::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream >> username >> password;
    return true;
}

ChannelMessage::ChannelMessage()
{
    messageType = cmtChannelMessage;
}

bool ChannelMessage::pack(QDataStream &stream) const
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream << sender << receiver << messageText;
    return true;
}

bool ChannelMessage::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream >> sender >> receiver >> messageText;
    return true;
}
