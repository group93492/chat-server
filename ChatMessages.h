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
    Q_PROPERTY(quint8 messageType READ getMessageType)
protected:
    quint8 m_messageType;
public:
    explicit ChatMessageBody();
    quint8 getMessageType();
};

class ChatMessageHeader: public Serializable
{
    Q_OBJECT
    Q_PROPERTY(quint8 messageType READ getMessageType WRITE setMessageType)
    Q_PROPERTY(quint32 messageSize READ getMessageSize WRITE setMessageSize)
protected:
    quint8 m_messageType;
    quint32 m_messageSize;
public:
    explicit ChatMessageHeader();
    quint8 getMessageType();
    void setMessageType(const quint8 type);
    quint32 getMessageSize();
    void setMessageSize(const quint32 size);
};

class AuthorizationAnswer : public ChatMessageBody
{
    Q_OBJECT
    Q_PROPERTY(quint8 messageType READ getMessageType)
    Q_PROPERTY(bool authorizationResult READ getAuthorizationResult WRITE setAuthorizationResult)
    Q_PROPERTY(QString denialReason READ getDenialReason WRITE setDenialReason)
protected:
    bool m_authorizationResult;
    QString m_denialReason;
public:
    explicit AuthorizationAnswer();
    bool getAuthorizationResult();
    void setAuthorizationResult(const bool result);
    QString &getDenialReason();
    void setDenialReason(const QString &reason);
};

class AuthorizationRequest: public ChatMessageBody
{
    Q_OBJECT
    Q_PROPERTY(quint8 messageType READ getMessageType)
    Q_PROPERTY(QString username READ getUsername WRITE setUsername)
    Q_PROPERTY(QString password READ getPassword WRITE setPassword)
protected:
    QString m_username;
    QString m_password;
public:
    explicit AuthorizationRequest();
    QString &getUsername();
    void setUsername(const QString &un);
    QString &getPassword();
    void setPassword(const QString &pw);
};

class ChannelMessage : public ChatMessageBody
{
    Q_OBJECT
    Q_PROPERTY(quint8 messageType READ getMessageType)
    Q_PROPERTY(QString sender READ getSender WRITE setSender)
    Q_PROPERTY(QString receiver READ getReceiver WRITE setReceiver)
    Q_PROPERTY(QString messageText READ getMessageText WRITE setMessageText)
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

};

#endif // CHATMESSAGES_H
