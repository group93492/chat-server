#ifndef CHATMESSAGES_H
#define CHATMESSAGES_H

#include <QDataStream>

enum ChatMessageType
{
    cmtUnknown,
    cmtChannelMessage,
    cmtAuthorizationRequest,
    cmtAuthorizationAnswer
    /*etc*/
};

class ChatMessageBody
{
public:
    ChatMessageBody();
    quint8 messageType;
    virtual bool pack(QDataStream &stream);
    virtual bool unpack(QDataStream &stream);
};

class ChatMessageHeader
{
public:
    ChatMessageHeader();
    ChatMessageHeader(ChatMessageBody *msgBody);
    quint8 messageType;
    quint32 messageSize;
    bool pack(QDataStream &stream);
    bool unpack(QDataStream &stream);
};

class AuthorizationAnswer : public ChatMessageBody
{
public:
    AuthorizationAnswer();
    bool authorizationResult;
    QString denialReason;
    virtual bool pack(QDataStream &stream);
    virtual bool unpack(QDataStream &stream);
};

class AuthorizationRequest: public ChatMessageBody
{
public:
    AuthorizationRequest();
    QString username;
    QString password;
    virtual bool pack(QDataStream &stream);
    virtual bool unpack(QDataStream &stream);
};

class ChannelMessage : public ChatMessageBody
{
public:
    ChannelMessage();
    QString sender;
    QString receiver;
    QString messageText;
    virtual bool pack(QDataStream &stream);
    virtual bool unpack(QDataStream &stream);
};

#endif // CHATMESSAGES_H
