#include "ChatMessages.h"

ChatMessageHeader::ChatMessageHeader(const ChatMessageBody *msgBody)
{
    messageType = msgBody->messageType;
    messageSize = sizeof(*msgBody);
}

ChatMessageHeader::ChatMessageHeader(QDataStream &stream)
{
    unpack(stream);
    //bad news that we will fault if stream.status is not ok because
    //we hadn't any chance to notice about it through constructor
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

AuthorizationAnswer::AuthorizationAnswer(QDataStream &stream)
{
    messageType = cmtAuthorizationAnswer;
    unpack(stream);
    //bad news that we will fault if stream.status is not ok because
    //we hadn't any chance to notice about it through constructor
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

AuthorizationRequest::AuthorizationRequest(QDataStream &stream)
{
    messageType = cmtAuthorizationRequest;
    unpack(stream);
    //bad news that we will fault if stream.status is not ok because
    //we hadn't any chance to notice about it through constructor
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

ChannelMessage::ChannelMessage(QDataStream &stream)
{
    messageType = cmtChannelMessage;
    unpack(stream);
    //bad news that we will fault if stream.status is not ok because
    //we hadn't any chance to notice about it through constructor
    //- one more lack of overloaded constructor
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

DisconnectMessage::DisconnectMessage()
{
    messageType = cmtDisconnectMessage;
}

DisconnectMessage::DisconnectMessage(QDataStream &stream)
{
    messageType = cmtDisconnectMessage;
    unpack(stream);
}

bool DisconnectMessage::pack(QDataStream &stream) const
{
    if (stream.status() != QDataStream::Ok)
            return false;
    stream << sender;
    return true;
}

bool DisconnectMessage::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream >> sender;
    return true;
}

RegistrationRequest::RegistrationRequest()
{
    messageType = cmtRegistrationRequest;
}

RegistrationRequest::RegistrationRequest(QDataStream &stream)
{
    messageType = cmtRegistrationRequest;
    unpack(stream);
}

bool RegistrationRequest::pack(QDataStream &stream) const
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream << username << password;
    return true;
}

bool RegistrationRequest::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream >> username >> password;
    return true;
}

RegistrationAnswer::RegistrationAnswer()
{
    messageType = cmtRegistrationAnswer;
}

RegistrationAnswer::RegistrationAnswer(QDataStream &stream)
{
    messageType = cmtRegistrationAnswer;
    unpack(stream);
}

bool RegistrationAnswer::pack(QDataStream &stream) const
{
    if (stream.status() != QDataStream::Ok)
        return false;
    if(registrationResult)
        stream << registrationResult;
    else
        stream << registrationResult << denialReason;
    return true;
}

bool RegistrationAnswer::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream >> registrationResult;
    if(!registrationResult)
        stream >> denialReason;
    return true;
}

ChannelListMessage::ChannelListMessage()
{
    messageType = cmtChannelListMessage;
}

ChannelListMessage::ChannelListMessage(QDataStream &stream)
{
    messageType = cmtChannelListMessage;
    unpack(stream);
}

bool ChannelListMessage::pack(QDataStream &stream) const
{
}

bool ChannelListMessage::unpack(QDataStream &stream)
{
}

ChannelInfo::ChannelInfo()
{
    messageType = cmtChannelInfo;
}

ChannelInfo::ChannelInfo(QDataStream &stream)
{
    messageType = cmtChannelInfo;
    unpack(stream);
}

bool ChannelInfo::pack(QDataStream &stream) const
{
}

bool ChannelInfo::unpack(QDataStream &stream)
{
}
