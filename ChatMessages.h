#ifndef CHATMESSAGES_H
#define CHATMESSAGES_H

#include <QDataStream>
#include <QString>
#include <QStringList>

enum ChatMessageType
{
    cmtUnknown,
    cmtChannelMessage,
    cmtAuthorizationRequest,
    cmtAuthorizationAnswer,
    cmtDisconnectMessage,
    cmtRegistrationRequest,
    cmtRegistrationAnswer,
    cmtChannelList,
    cmtChannelInfo/*,
    cmtChannelJoinRequest,
    cmtChannelJoinResult*/
    /*etc*/
};

class ChatMessageBody
{
public:
    ChatMessageBody() {}
    virtual ~ChatMessageBody() {}
    quint8 messageType;
    virtual bool pack(QDataStream &stream) const = 0;
    virtual bool unpack(QDataStream &stream) = 0;
};

class ChatMessageHeader
{
public:
    ChatMessageHeader() {}
    ChatMessageHeader(const ChatMessageBody *msgBody);
    ChatMessageHeader(QDataStream &stream);
    quint8 messageType;
    quint32 messageSize;
    bool pack(QDataStream &stream) const;
    bool unpack(QDataStream &stream);
};

class AuthorizationAnswer : public ChatMessageBody
{
public:
    AuthorizationAnswer();
    AuthorizationAnswer(QDataStream &stream);
    bool authorizationResult;
    QString denialReason;
    bool pack(QDataStream &stream) const;
    bool unpack(QDataStream &stream);
};

class AuthorizationRequest: public ChatMessageBody
{
public:
    AuthorizationRequest();
    AuthorizationRequest(QDataStream &stream);
    QString username;
    QString password;
    bool pack(QDataStream &stream) const;
    bool unpack(QDataStream &stream);
};

class ChannelMessage : public ChatMessageBody
{
public:
    ChannelMessage();
    ChannelMessage(QDataStream &stream);
    QString sender;
    QString receiver;
    QString messageText;
    bool pack(QDataStream &stream) const;
    bool unpack(QDataStream &stream);
};

class DisconnectMessage : public ChatMessageBody
{
public:
    DisconnectMessage();
    DisconnectMessage(QDataStream &stream);
    QString sender;
    bool pack(QDataStream &stream) const;
    bool unpack(QDataStream &stream);
};

class RegistrationRequest: public ChatMessageBody
{
public:
    RegistrationRequest();
    RegistrationRequest(QDataStream &stream);
    QString username;
    QString password;
    bool pack(QDataStream &stream) const;
    bool unpack(QDataStream &stream);
};

class RegistrationAnswer: public ChatMessageBody
{
public:
    RegistrationAnswer();
    RegistrationAnswer(QDataStream &stream);
    bool registrationResult;
    QString denialReason;
    bool pack(QDataStream &stream) const;
    bool unpack(QDataStream &stream);
};

class ChannelList: public ChatMessageBody
{
public:
    enum ListType
    {
        listOfJoined,
        listOfAll
    };
    ChannelList();
    ChannelList(QDataStream &stream);
    ListType listType;
    QStringList channelList;
    bool pack(QDataStream &stream) const;
    bool unpack(QDataStream &stream);
};

//maybe server should send information about every channel in the channel list, not in the other message?
class ChannelInfo: public ChatMessageBody
{
public:
    ChannelInfo();
    ChannelInfo(QDataStream &stream);
    QString channelName;
    QString channelDescription;
    QString channelTopic;
    bool pack(QDataStream &stream) const;
    bool unpack(QDataStream &stream);
};

#endif // CHATMESSAGES_H
