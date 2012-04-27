#include "ChatMessages.h"

ChatMessageHeader::ChatMessageHeader(const ChatMessageBody *msgBody)
{
    messageType = msgBody->messageType;
    messageSize = sizeof(*msgBody);
    streamStatus = Ok;
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
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream << messageType;
    return true;
}

bool ChatMessageHeader::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
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
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    if(authorizationResult)
        stream << authorizationResult;
    else
        stream << authorizationResult << denialReason;
    return true;
}

bool AuthorizationAnswer::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
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
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream << username << password;
    return true;
}

bool AuthorizationRequest::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
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
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream << sender << receiver << messageText;
    return true;
}

bool ChannelMessage::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
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
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream << sender;
    return true;
}

bool DisconnectMessage::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
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
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream << username << password;
    return true;
}

bool RegistrationRequest::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
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
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    if(registrationResult)
        stream << registrationResult;
    else
        stream << registrationResult << denialReason;
    return true;
}

bool RegistrationAnswer::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
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
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream << quint8(listType) << channelList;
    return true;
}

bool ChannelListMessage::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    quint8 tempType;
    stream >> tempType >> channelList;
    listType = (ChannelListMessage::ListType) tempType;
    return true;
}

ChannelInfoMessage::ChannelInfoMessage()
{
    messageType = cmtChannelInfo;
}

ChannelInfoMessage::ChannelInfoMessage(QDataStream &stream)
{
    messageType = cmtChannelInfo;
    unpack(stream);
}

bool ChannelInfoMessage::pack(QDataStream &stream) const
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream << channelName << channelDescription << channelTopic;
    stream << channelMembers;
    return true;
}

bool ChannelInfoMessage::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream >> channelName >> channelDescription >> channelTopic;
    stream >> channelMembers;
    return true;
}

bool ChannelListRequest::pack(QDataStream &stream) const
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream << quint8(listType) << nick;
    return true;
}

bool ChannelListRequest::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    quint8 tempType;
    stream >> tempType >> nick;
    listType = (ChannelListRequest::ListType) tempType;
    return true;
}

ChannelListRequest::ChannelListRequest()
{
    messageType = cmtChannelListRequest;
}

ChannelListRequest::ChannelListRequest(QDataStream &stream)
{
    messageType = cmtChannelListRequest;
    unpack(stream);
}

ChannelJoinRequest::ChannelJoinRequest()
{
    messageType = cmtChannelJoinRequest;
}

ChannelJoinRequest::ChannelJoinRequest(QDataStream &stream)
{
    messageType = cmtChannelJoinRequest;
    unpack(stream);
}

bool ChannelJoinRequest::pack(QDataStream &stream) const
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream << nick << channelName;
    return true;
}

bool ChannelJoinRequest::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream >> nick >> channelName;
    return true;
}

ChannelJoinResult::ChannelJoinResult()
{
    messageType = cmtChannelJoinResult;
}

ChannelJoinResult::ChannelJoinResult(QDataStream &stream)
{
    messageType = cmtChannelJoinResult;
    unpack(stream);
}

bool ChannelJoinResult::pack(QDataStream &stream) const
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream << result << channelName;
    return true;
}

bool ChannelJoinResult::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream >> result >> channelName;
    return true;
}

ChannelLeaveMessage::ChannelLeaveMessage()
{
    messageType = cmtChannelLeaveMessage;
}

ChannelLeaveMessage::ChannelLeaveMessage(QDataStream &stream)
{
    messageType = cmtChannelLeaveMessage;
    unpack(stream);
}

bool ChannelLeaveMessage::pack(QDataStream &stream) const
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream << nick << channelName;
    return true;
}

bool ChannelLeaveMessage::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream >> nick >> channelName;
    return true;
}

ChannelSystemMessage::ChannelSystemMessage()
{
    messageType = cmtChannelSystemMessage;
}

ChannelSystemMessage::ChannelSystemMessage(QDataStream &stream)
{
    messageType = cmtChannelSystemMessage;
    unpack(stream);
}

bool ChannelSystemMessage::pack(QDataStream &stream) const
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream << msg;
    return true;
}

bool ChannelSystemMessage::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
    {
        Status = Failed;
        return false;
    }
    else
        Status = Ok;
    stream >> msg;
    return true;
}

