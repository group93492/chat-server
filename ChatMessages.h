#ifndef CHATMESSAGES_H
#define CHATMESSAGES_H

#include <QObject>

enum ChatMessageType
{
    cmtUnknown,
    cmtChannelMessage,
    cmtAuthorizationRequest,
    cmtAuthorizationAnswer
    /*etc*/
};

class ChatMessageBody: public QObject
{
    Q_OBJECT
protected:
    quint8 m_messageType;
public:
    explicit ChatMessageBody();
    quint8 getMessageType();
    virtual bool pack(QDataStream &stream);
    virtual bool unpack(QDataStream &stream);
};

class ChatMessageHeader: public QObject
{
    Q_OBJECT
protected:
    quint8 m_messageType;
    quint32 m_messageSize;
public:
    explicit ChatMessageHeader();
    quint8 getMessageType();
    void setMessageType(const quint8 type);
    quint32 getMessageSize();
    void setMessageSize(const quint32 size);
    bool pack(QDataStream &stream);
    bool unpack(QDataStream &stream);
};

class AuthorizationAnswer : public ChatMessageBody
{
    Q_OBJECT
protected:
    bool m_authorizationResult;
    QString m_denialReason;
public:
    explicit AuthorizationAnswer();
    bool getAuthorizationResult();
    void setAuthorizationResult(const bool result);
    QString &getDenialReason();
    void setDenialReason(const QString &reason);
    virtual bool pack(QDataStream &stream);
    virtual bool unpack(QDataStream &stream);
};

class AuthorizationRequest: public ChatMessageBody
{
    Q_OBJECT
protected:
    QString m_username;
    QString m_password;
public:
    explicit AuthorizationRequest();
    QString &getUsername();
    void setUsername(const QString &un);
    QString &getPassword();
    void setPassword(const QString &pw);
    virtual bool pack(QDataStream &stream);
    virtual bool unpack(QDataStream &stream);
};

class ChannelMessage : public ChatMessageBody
{
    Q_OBJECT
protected:
    QString m_sender;
    QString m_receiver;
    QString m_messageText;
public:
    explicit ChannelMessage();
    QString &getSender();
    void setSender(const QString &sender);
    QString &getReceiver();
    void setReceiver(const QString &receiver);
    QString &getMessageText();
    void setMessageText(const QString &text);
    virtual bool pack(QDataStream &stream);
    virtual bool unpack(QDataStream &stream);
};

#endif // CHATMESSAGES_H
