#ifndef CHATMESSAGES_H
#define CHATMESSAGES_H

#include <QObject>

enum ChatMessageType
{
    cmtInformationalMessage,
    cmtAuthorizationRequest,
    cmtAuthorizationAnswer
    /*etc*/
};

class Serializable: public QObject
{
    Q_OBJECT
public:
    explicit Serializable();
    bool pack(QDataStream &stream);
    bool unpack(QDataStream &stream);
};

class ChatMessageBody: public Serializable
{
    Q_OBJECT
public:
    explicit ChatMessageBody();
};

class ChatMessageHeader: public Serializable
{
    Q_OBJECT
public:
    explicit ChatMessageHeader();
    quint8 messageType;
    quint32 messageSize;
};

class AuthorizationAnswer : public ChatMessageBody
{
    Q_OBJECT
public:
    explicit AuthorizationAnswer();
    bool authorizationResult;
};

class AuthorizationRequest: public ChatMessageBody
{
    Q_OBJECT
public:
    explicit AuthorizationRequest();
    QString username;
    QString password;
};

class InformationalMessage : public ChatMessageBody
{
    Q_OBJECT
public:
    explicit InformationalMessage();
    QString sender;
    QString receiver;
    QString messageBody;
};

#endif // CHATMESSAGES_H
